#include "ak.h"
#include "AKProcess.h"

namespace ak
{
    AKProcess::AKProcess(
            AKSystem *system,//系统对象
            AKCore *core,//核对象
            const int8 *scriptUrl//脚本url
    )
            :mSystem(system),
             mCore(core),
             mScript(system,core,this),
             mRef(0),
             mBindParent(NULL),
             mRecvMsgBuf1(core),
             mRecvMsgBuf2(core),
             mRecvMsgBuf3(core),
             mProcessVarDict(core),
             mT0(0),
             mRunBeginTime(0),
             mIsNoBreakPoint(false)
    {
        mScript.setUrl(scriptUrl);
    }

    AKProcess::~AKProcess()
    {
        {
            AKProcessMap::iterator it=mBindProcessMap.begin();
            while(it!=mBindProcessMap.end())
            {
                mSystem->destroyProcess(it->second);
                ++it;
            }
        }

    }

    void AKProcess::retain(
            uint32 count//引用次数
    )
    {
        if(count==0)
        {
            count=1;
        }

        AKUseMutex use(mRefMutex);
        //mRefMutex.lock();
        mRef+=count;
        //mRefMutex.unlock();
    }

    void AKProcess::release()
    {
        mRefMutex.lock();
        --mRef;
        mRefMutex.unlock();
        if(mRef==0)
        {
            if(mBindParent&&
               mCore->checkProcess(mBindParent)
                    )
            {
                mBindParent->rebind(this);
            }
            delete this;
        }
    }

    void AKProcess::bind(AKProcess *process)
    {
        if(mCore->checkProcess(process))
        {
            process->mBindParent=this;
            mBindProcessMap[process]=process;
        }
    }

    void AKProcess::rebind(AKProcess *process)
    {
        mBindProcessMap.erase(process);
    }

    //获取向进程返回的response数据
    AKProcess *AKProcess::getResponse(
            container::AKBuf &msg//msg缓冲
    )
    {
        uint32 msgSize=0;
        AKProcess *process=NULL;
        AKUseMutex use(mRecvMsgBuf3Mutex);
        bool rt=mRecvMsgBuf3.read((int8 *)&process,sizeof(void *));//进程句柄
        if(!rt)
        {
            return NULL;
        }
        mRecvMsgBuf3.readUint32(msgSize);//msg尺寸
        msg.clear();
        uint32 pos=mRecvMsgBuf3.getPos();
        msg.append(mRecvMsgBuf3,pos,msgSize);//读取msg内容
        msg.setPos(0);
        mRecvMsgBuf3.clear();
        return process;
    }

    //向该进程返回数据
    void AKProcess::response(
            AKProcess *process,//发送者进程
            container::AKBuf **msgDataArray,//msg数据缓冲数组
            uint32 msgDataSize//msg数据缓冲数量
    )
    {
        uint32 i;
        uint32 msgSize=0;
        for(i=0;i<msgDataSize;++i)
        {
            msgSize+=msgDataArray[i]->size();
        }

        AKUseMutex use(mRecvMsgBuf3Mutex);
        mRecvMsgBuf3.clear();//清空原来的数据
        uint32 pos=mRecvMsgBuf3.getPos();
        mRecvMsgBuf3.setEnd();
        mRecvMsgBuf3.write((const int8 *)&process,sizeof(void *));//进程句柄
        mRecvMsgBuf3.writeUint32(msgSize);//msg尺寸

        //msg内容
        for(i=0;i<msgDataSize;++i)
        {
            mRecvMsgBuf3.append(*msgDataArray[i]);
        }
        mRecvMsgBuf3.setPos(pos);
    }

    //向该进程发送消息并请求数据
    void AKProcess::request(
            AKProcess *process,//发送者进程
            container::AKBuf **msgDataArray,//msg数据缓冲数组
            uint32 msgDataSize//msg数据缓冲数量
    )
    {
        AKUseMutex use(mRecvMsgBuf3Mutex);
        mRecvMsgBuf3.clear();

        uint32 i;
        uint32 msgSize=0;
        for(i=0;i<msgDataSize;++i)
        {
            msgSize+=msgDataArray[i]->size();
        }

        AKUseMutex use2(mRecvMsgBuf2Mutex);
        uint32 pos=mRecvMsgBuf2.getPos();
        mRecvMsgBuf2.setEnd();
        mRecvMsgBuf2.write((const int8 *)&process,sizeof(void *));//进程句柄
        mRecvMsgBuf2.writeUint32(msgSize);//msg尺寸

        //msg内容
        for(i=0;i<msgDataSize;++i)
        {
            mRecvMsgBuf2.append(*msgDataArray[i]);
        }
        mRecvMsgBuf2.setPos(pos);
    }

    //检查是否收到进程msg请求数据
    AKProcess *AKProcess::hasRequest(
            container::AKBuf &msg//msg缓冲
    )
    {
        uint32 msgSize=0;
        AKProcess *process=NULL;
        AKUseMutex use(mRecvMsgBuf2Mutex);
        bool rt=mRecvMsgBuf2.read((int8 *)&process,sizeof(void *));//进程句柄
        if(!rt)
        {
            return NULL;
        }
        mRecvMsgBuf2.readUint32(msgSize);//msg尺寸
        msg.clear();
        uint32 pos=mRecvMsgBuf2.getPos();
        msg.append(mRecvMsgBuf2,pos,msgSize);//读取msg内容
        pos+=msgSize;
        if(pos==mRecvMsgBuf2.size())
        {
            mRecvMsgBuf2.setEnd();
        }
        else
        {
            mRecvMsgBuf2.setPos(pos);
        }
        uint32 needPopCount=pos/AKBUF_BLOCK_SIZE;
        for(uint32 i=0;i<needPopCount;++i)
        {
            mRecvMsgBuf2.popBlock();
        }
        return process;
    }

    //向该进程发送msg
    void AKProcess::send(
            AKProcess *process,//发送者进程
            container::AKBuf &msg//msg缓冲
    )
    {
        AKUseMutex use(mRecvMsgBuf1Mutex);
        uint32 pos=mRecvMsgBuf1.getPos();
        mRecvMsgBuf1.setPos(mRecvMsgBuf1.size());
        mRecvMsgBuf1.write((const int8 *)&process,sizeof(void *));//进程句柄
        mRecvMsgBuf1.writeUint32(msg.size());//msg尺寸
        mRecvMsgBuf1.append(msg);//msg内容
        mRecvMsgBuf1.setPos(pos);
    }

    //向该进程发送msg
    void AKProcess::send(
            AKProcess *process,//发送者进程
            container::AKBuf **msgDataArray,//msg数据缓冲数组
            uint32 msgDataSize//msg数据缓冲数量
    )
    {
        uint32 i;
        uint32 msgSize=0;
        for(i=0;i<msgDataSize;++i)
        {
            msgSize+=msgDataArray[i]->size();
        }

        AKUseMutex use(mRecvMsgBuf1Mutex);
        uint32 pos=mRecvMsgBuf1.getPos();
        mRecvMsgBuf1.setEnd();
        mRecvMsgBuf1.write((const int8 *)&process,sizeof(void *));//进程句柄
        mRecvMsgBuf1.writeUint32(msgSize);//msg尺寸

        //msg内容
        for(i=0;i<msgDataSize;++i)
        {
            mRecvMsgBuf1.append(*msgDataArray[i]);
        }
        mRecvMsgBuf1.setPos(pos);
    }

    //接收进程msg
    AKProcess *AKProcess::recv(
            container::AKBuf &msg//msg缓冲
    )
    {
        uint32 msgSize=0;
        AKProcess *process=NULL;
        AKUseMutex use(mRecvMsgBuf1Mutex);
        bool rt=mRecvMsgBuf1.read((int8 *)&process,sizeof(void *));//进程句柄
        if(!rt)
        {
            return NULL;
        }
        mRecvMsgBuf1.readUint32(msgSize);//msg尺寸
        msg.clear();
        uint32 pos=mRecvMsgBuf1.getPos();
        msg.append(mRecvMsgBuf1,pos,msgSize);//读取msg内容
        pos+=msgSize;
        if(pos==mRecvMsgBuf1.size())
        {
            mRecvMsgBuf1.setEnd();
        }
        else
        {
            mRecvMsgBuf1.setPos(pos);
        }
        uint32 needPopCount=pos/AKBUF_BLOCK_SIZE;
        for(uint32 i=0;i<needPopCount;++i)
        {
            mRecvMsgBuf1.popBlock();
        }
        return process;
    }

    //执行脚本：返回false代表脚本执行完成，否则代表未完成
    bool AKProcess::run(
            uint32 executeOrderCount
    )
    {
        mRunBeginTime=AKCommon::time_milli();
        return mScript.run(executeOrderCount);
    }

    void AKProcess::stop()
    {
        mScript.stop();
    }

}
