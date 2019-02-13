#include "ak.h"
#include "AKSystem.h"
#include "AKGlobal.h"

namespace ak
{
    AKSystem::AKSystem(
            uint32 coreCount//核数
    )
            :mIsQuit(false),
             mCoreCount((coreCount<1?1:coreCount)+1),//预留一个系统任务进程使用的核（该核ID为最大）
             mAutoSleepTime(1)
    {
        mCoreArray.resize(1);
        for(uint32 id=1;id<=mCoreCount;++id)
        {
            AKCore *core=new AKCore(this,id);
            mCoreArray.push_back(core);
        }
        mIO.post(boost::bind(&AKSystem::asynStart,this));
    }

    AKSystem::~AKSystem()
    {
        uint32 coreCount=getCoreCount();
        for(uint32 id=1;id<=coreCount;++id)
        {
            delete mCoreArray[id];
        }
    }

    void AKSystem::asynStart()
    {
        for(uint32 id=1;id<=mCoreCount;++id)
        {
            AKCore *core=mCoreArray[id];
            boost::thread *thread=new boost::thread(boost::bind(&AKSystem::coreThread,this,id));
            mThreadList.push_back(AKThreadPtr(thread));
        }
        mIO.post(boost::bind(&AKSystem::asynUpdate,this));
    }

    void AKSystem::coreThread(
            uint32 coreID
    )
    {
        AKCore *core=mCoreArray[coreID];
        core->getIO().run();
    }

    void AKSystem::quit()
    {
        uint32 coreCount=mCoreArray.size()-1;
        for(uint32 id=1;id<=coreCount;++id)
        {
            AKCore *core=mCoreArray[id];
            core->quit();
        }

        mIO.post(boost::bind(&AKSystem::asynQuit,this));
    }

    void AKSystem::asynQuit()
    {
        {
            //等待核线程结束
            AKThreadList::iterator it=mThreadList.begin();
            while(it!=mThreadList.end())
            {
                (*it)->join();
                ++it;
            }
        }

        mIsQuit=true;
    }

    boost::asio::io_service &AKSystem::getIO()
    {
        return mIO;
    }

    uint32 AKSystem::getCoreCount()
    {
        return mCoreArray.size()-1;
    }

    uint32 AKSystem::getFreeCoreID()
    {
        uint32 coreID=0;
        uint32 processCount=0;
        uint32 coreCount=getCoreCount()-1;//系统任务使用的核不能分配
        for(uint32 id=1;id<=coreCount;++id)
        {
            AKCore *core=mCoreArray[id];
            if(coreID==0)
            {
                coreID=id;
                processCount=core->mCoreProcessInfoMap.size();
            }
            else
            {
                uint32 curCount=core->mCoreProcessInfoMap.size();
                if(curCount<processCount)
                {
                    coreID=id;
                    processCount=curCount;
                }
            }
        }
        return coreID;
    }

    AKProcess *AKSystem::createProcess(
            const int8 *scriptUrl,//脚本url
            uint32 coreID,//核id（0=自动分配）
            container::AKBuf *buf//数据缓冲
    )
    {
        uint32 coreCount=getCoreCount();

        //分配核
        if(coreID==0||coreID>coreCount)
        {
            coreID=getFreeCoreID();
        }

        AKProcess *process=new AKProcess(this,mCoreArray[coreID],scriptUrl);
        if(buf)
        {
            process->mScript.mDataBuf.append(*buf);
        }
        process->retain(getCoreCount());

        AKProcessInfo info;
        info.coreID=coreID;
        info.process=process;


        for(uint32 id=1;id<=coreCount;++id)
        {
            AKCore *core=mCoreArray[id];
            core->getIO().post(boost::bind(&AKCore::asynCreateProcess,core,info));
        }

        return process;
    }

    void AKSystem::destroyProcess(//销毁进程
            AKProcess *process//进程对象
    )
    {
        uint32 coreCount=getCoreCount();
        for(uint32 id=1;id<=coreCount;++id)
        {
            AKCore *core=mCoreArray[id];
            core->getIO().post(boost::bind(&AKCore::asynDestroyProcess,core,process));
        }
    }

    void AKSystem::asynUpdate()
    {
        update(mUpdateTimer.elapsed()*1000);
        mUpdateTimer.restart();

        if(!mIsQuit)
        {
            mIO.post(boost::bind(&AKSystem::asynUpdate,this));
        }
    }

    void AKSystem::update(double dt)
    {
        if(this->mAutoSleepTime>=0)
        {
            AKCommon::sleep(this->mAutoSleepTime);
        }
    }

    void AKSystem::onPreError(
            AKScript *script,
            const int8 *error
    )
    {
        /////////////////////////////////////////////////////////////////////////////////////
        std::string strError;
        strError+=AKCommon::now();//当前日期时间
        strError+="\r\n";
        //获取调用堆栈
        std::string strCallStack;
        std::vector<AKScript *> callStackArr;
        script->getCallStackArr(callStackArr);
        int32 c=callStackArr.size();
        for(int32 i=0;i<c;++i)
        {
            AKScript *curScript=callStackArr[i];
            //脚本函数路径
            strCallStack+=curScript->getUrl();
            /////////////////////////////////////////////////////////////////////////////////////
            //正在执行的指令
            uint32 orderCount=curScript->mOrderArray.size();
            uint32 orderPos=curScript->mOrderPos;
            if(orderPos<orderCount)
            {
                AKOrder *order=curScript->mOrderArray[orderPos];
                strCallStack+=" at row ";
                strCallStack+=order->rowNum;
                //输出指令码
                AKStringArray &params=order->params;
                strCallStack+=" ";
                strCallStack+=params[1];
                strCallStack+="=";
                strCallStack+=params[0];
                strCallStack+="(";
                uint32 c=params.size();
                for(uint32 i=2;i<c;++i)
                {
                    if(i>2)strCallStack+=",";
                    strCallStack+=params[i];
                }
                strCallStack+=");";
            }
            else
            {
            }
            /////////////////////////////////////////////////////////////////////////////////////
            strCallStack+="\r\n";
        }
        strError+=error;
        strError+="\r\n";
        strError+=strCallStack;
        /////////////////////////////////////////////////////////////////////////////////////
        if(AKGlobal::getSingleton().mErrorLogFilePath!="")
        {
            //记录错误日志
            FILE *fp=fopen(AKGlobal::getSingleton().mErrorLogFilePath.c_str(),"ab");
            if(fp)
            {
                fwrite(strError.c_str(),strError.size(),1,fp);
                fwrite("\r\n",2,1,fp);
                fclose(fp);
            }
        }

        if(!script->tryCatch(strError.c_str()))//处理异常失败
        {
            script->getSystem()->onRunError(strError.c_str());
        }
    }

}
