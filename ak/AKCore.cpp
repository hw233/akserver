#include "ak.h"
#include "AKCore.h"
#include "AKGlobal.h"
#include "AKHandleBuf.h"
#include "AKHandleProcess.h"

namespace ak
{
    AKCore::AKCore(
            AKSystem *system,//系统对象
            uint32 id//核id
    )
            :mSystem(system),
             mID(id),
             mIsQuit(false),
             mGlobalVarDict(this),
             mIsDestroy(false)
    {
        mIO.post(boost::bind(&AKCore::asynUpdate,this));
    }

    AKCore::~AKCore()
    {
        {
            AKProcessInfoMap::iterator it=mCoreProcessInfoMap.begin();
            while(it!=mCoreProcessInfoMap.end())
            {
                delete it->first;
                ++it;
            }
            mCoreProcessInfoMap.clear();
        }

        {
            AKClientMap::iterator it=mClientMap.begin();
            while(it!=mClientMap.end())
            {
                delete it->second;
                ++it;
            }
            mClientMap.clear();
        }

        {
            AKServerMap::iterator it=mServerMap.begin();
            while(it!=mServerMap.end())
            {
                delete it->second;
                ++it;
            }
            mServerMap.clear();
        }

        {
            AKRefMap::iterator it=mRefMap.begin();
            while(it!=mRefMap.end())
            {
                delete it->second;
                ++it;
            }
            mRefMap.clear();
        }


        mIsDestroy=true;//标记被销毁
    }

    uint32 AKCore::getID()
    {
        return mID;
    }

    bool AKCore::checkClient(net::AKClient *client)
    {
        AKClientMap::iterator it=mClientMap.find(client);
        if(it==mClientMap.end())return false;
        return true;
    }

    net::AKClient *AKCore::createClient(
            AKProcess *process,//进程
            const int8 *domain,//域名或ip
            uint32 port,//端口
            uint32 recvBufSize//接收缓冲尺寸
    )
    {
        net::AKClient *client=new net::AKClient(mSystem,this,process,domain,port,recvBufSize);
        mClientMap[client]=client;

        return client;
    }

    void AKCore::removeClient(net::AKClient *client)
    {
        mClientMap.erase(client);
        client->mIsDeleted=true;//标记删除
    }

    bool AKCore::checkServer(net::AKServer *server)
    {
        AKServerMap::iterator it=mServerMap.find(server);
        if(it==mServerMap.end())return false;
        return true;
    }

    net::AKServer *AKCore::createServer(
            AKProcess *process,//进程
            const int8 *bindIP,//绑定ip或域名
            uint32 port,//端口
            uint32 clientMaxCount,//最大client数量
            uint32 keepAcceptCount,//保持accept数量
            uint32 recvBufSize//接收缓冲尺寸
    )
    {
        net::AKServer *server=new net::AKServer(mSystem,this,process,port,bindIP,clientMaxCount,keepAcceptCount,recvBufSize);
        mServerMap[server]=server;

        return server;
    }



    net2::AKClientForScript *AKCore::createClient2()
    {
        net2::AKClientForScript *client=new net2::AKClientForScript(getIO(),this);
        clientMap[client]=client;
        return client;
    }

    bool AKCore::checkClient2(net2::AKClientForScript *client)
    {
        if(clientMap.find(client)==clientMap.end())return false;
        return true;
    }

    void AKCore::destroyClient2(net2::AKClientForScript *client)
    {
        clientMap.erase(client);
    }

    net2::AKServerForScript *AKCore::createServer2(
            const int8 *bindDomain,//绑定ip或域名
            uint16 port//端口
    )
    {
        net2::AKServerForScript *server=NULL;
        if(strcmp(bindDomain,"")==0)
        {
            server=new net2::AKServerForScript(getIO(),port,this);
        }
        else
        {
            server=new net2::AKServerForScript(getIO(),bindDomain,port,this);
        }
        serverMap[server]=server;
        return server;
    }

    bool AKCore::checkServer2(net2::AKServerForScript *server)
    {
        if(serverMap.find(server)==serverMap.end())return false;
        return true;
    }

    boost::asio::io_service &AKCore::getIO()
    {
        return mIO;
    }

    void AKCore::quit()
    {
        mIO.post(boost::bind(&AKCore::asynQuit,this));
    }

    bool AKCore::checkProcess(AKProcess *process)
    {
        AKProcessInfoMap::iterator it=mProcessInfoMap.find(process);
        if(it==mProcessInfoMap.end())
        {
            return false;
        }
        return true;
    }

    bool AKCore::isQuit()
    {
        return mIsQuit;
    }

    void AKCore::asynQuit()
    {
        mIsQuit=true;
        asynStopAllProcessScript();
    }

    void AKCore::asynCreateProcess(AKProcessInfo info)
    {
        mProcessInfoMap[info.process]=info;
        if(info.coreID==mID)
        {
            mCoreProcessInfoMap[info.process]=info;
        }
    }

    void AKCore::asynDestroyProcess(AKProcess *process)
    {
        if(checkProcess(process))
        {
            mProcessInfoMap.erase(process);
            mCoreProcessInfoMap.erase(process);
            process->release();
        }
    }

    void AKCore::asynStopAllProcessScript()
    {
        AKProcessInfoMap::iterator it=mCoreProcessInfoMap.begin();
        while(it!=mCoreProcessInfoMap.end())
        {
            AKProcess *process=it->first;
            process->stop();
            ++it;
        }
    }

    void AKCore::update(double dt)
    {
        AKProcessInfoMap::iterator it=mCoreProcessInfoMap.begin();
        while(it!=mCoreProcessInfoMap.end())
        {
            AKProcess *process=it->first;
            if(mIsQuit)
            {
                process->stop();
            }
            try
            {
                if(process->run()==false)//进程脚本执行完成
                {
                    mSystem->destroyProcess(process);
                }
            }
            catch(const int8 *error)
            {
                mSystem->onRunError(error);
            }
            ++it;
        }

        if(this->getID()==mSystem->getCoreCount())//系统任务进程
        {
            AKCommon::sleep(10);
        }
        else if(mSystem->mAutoSleepTime>=0)
        {
            AKCommon::sleep(mSystem->mAutoSleepTime<0?0:mSystem->mAutoSleepTime);
        }

        //{
        //	//自动垃圾回收
        //	AKRefMap::iterator itOld;
        //	AKRefMap::iterator it=mRefMap.begin();
        //	while(it!=mRefMap.end())
        //	{
        //		AKRef *ref=it->second;
        //		itOld=it;
        //		++it;
        //		if(ref->getRefCount()==0)
        //		{
        //			mRefMap.erase(itOld);
        //			delete ref;
        //		}
        //	}
        //}
    }

    void AKCore::asynUpdate()
    {
        update(mUpdateTimer.elapsed()*1000);
        mUpdateTimer.restart();

        if((!mIsQuit)||
           mProcessInfoMap.size()>0
                )
        {
            mIO.post(boost::bind(&AKCore::asynUpdate,this));
        }
    }

    ak::container::AKStruct *AKCore::createStruct()
    {
        container::AKStruct *structObj=new container::AKStruct();
        structMap[structObj]=structObj;
        return structObj;
    }

    bool AKCore::checkStruct(ak::container::AKStruct *structObj)
    {
        if(structMap.find(structObj)==structMap.end())return false;
        return true;
    }

    void AKCore::destroyStruct(ak::container::AKStruct *structObj)
    {
        AKStructMap::iterator it=structMap.find(structObj);
        if(it==structMap.end())return;

        delete structObj;
        structMap.erase(it);
    }

    ak::container::AKBuf *AKCore::createBuf()
    {
        container::AKBuf *buf=new container::AKBuf(this);
        mRefMap[buf]=buf;
        return buf;
    }

    bool AKCore::checkBuf(AKRef *ref)
    {
        if(mRefMap.find(ref)!=mRefMap.end())
        {
            container::AKBuf *buf=dynamic_cast<container::AKBuf *>(ref);
            if(buf)return true;
        }
        return false;
    }

    container::AKArr *AKCore::createArr()
    {
        container::AKArr *arr=new container::AKArr(this);
        mRefMap[arr]=arr;
        return arr;
    }

    bool AKCore::checkArr(AKRef *ref)
    {
        if(mRefMap.find(ref)!=mRefMap.end())
        {
            container::AKArr *arr=dynamic_cast<container::AKArr *>(ref);
            if(arr)return true;
        }
        return false;
    }

    container::AKDict *AKCore::createDict()
    {
        container::AKDict *dict=new container::AKDict(this);
        mRefMap[dict]=dict;
        return dict;
    }

    bool AKCore::checkDict(AKRef *ref)
    {
        if(mRefMap.find(ref)!=mRefMap.end())
        {
            container::AKDict *dict=dynamic_cast<container::AKDict *>(ref);
            if(dict)return true;
        }
        return false;
    }

    void AKCore::destroyRef(AKRef *ref)
    {
        AKRefMap::iterator it=mRefMap.find(ref);
        if(it==mRefMap.end())return;

        delete ref;
        mRefMap.erase(it);
    }

    AKRef *AKCore::checkRef(void *handle)
    {
        if(mIsDestroy)return NULL;//防止AKGlobal被销毁后调用的检查引用有效性操作

        AKRefMap::iterator it=mRefMap.find((AKRef *)handle);
        if(it==mRefMap.end())return NULL;

        return (AKRef *)handle;
    }

    //获取当前脚本调用的子脚本函数
    AKScript *getCallChildScript(AKScript *script)
    {
        AKHandleProcess *handleProcess=(AKHandleProcess *)script->getHandler("AKHandleProcess");
        return handleProcess->mScriptFunc;
    }

    std::string AKCore::getRunTimeInfo()
    {
        int8 tmp[50];
        std::string info;
        AKProcessInfoMap::iterator it=mCoreProcessInfoMap.begin();
        while(it!=mCoreProcessInfoMap.end())
        {
            AKProcess *process=it->first;
            AKScript *script=&process->mScript;
            ////////////////////////////////////////////////////
            //获取当前调用的脚本函数
            while(true)
            {
                AKScript *childScript=getCallChildScript(script);
                if(!childScript)break;
                script=childScript;
            }
            ////////////////////////////////////////////////////
            //当前执行的指令

            ////////////////////////////////////////////////////
            //获取调用堆栈
            std::string strCallStack;
            std::vector<AKScript *> callStackArr;
            script->getCallStackArr(callStackArr);
            int32 c=callStackArr.size();
            for(int32 i=0;i<c;++i)
            {
                AKScript *curScript=callStackArr[i];
                if(i>0)strCallStack+="\r\n\r\n";
                //脚本函数路径
                strCallStack+=curScript->getUrl();
                /////////////////////////////////////////////////////////////////////////////////////
                //正在执行的指令
                uint32 orderCount=curScript->mOrderArray.size();
                uint32 orderPos=curScript->mOrderPos;
                if(orderPos<orderCount)
                {
                    AKOrder *order=curScript->mOrderArray[orderPos];
                    AKStringArray &params=order->params;
                    std::string &orderName=params[0];
                    //输出行号
                    strCallStack+=" ";
                    sprintf(tmp,"%lu",curScript->mOrderPos);
                    strCallStack+=tmp;
                    //输出指令码
                    strCallStack+=" ";
                    strCallStack+=params[1];
                    strCallStack+="=";
                    if(orderName!="=")strCallStack+=orderName;
                    if(orderName!="=")strCallStack+="(";
                    uint32 cc=params.size();
                    for(uint32 ii=2;ii<cc;++ii)
                    {
                        if(ii>2)strCallStack+=",";
                        strCallStack+=params[ii];
                    }
                    if(orderName!="=")strCallStack+=")";
                    strCallStack+=";";
                    //相关变量
                    for(uint32 ii=1;ii<cc;++ii)
                    {
                        std::string &varName=params[ii];
                        AKVar *curVar=curScript->getVar(varName.c_str());
                        if(curVar)
                        {
                            if(curVar->getType()==AKVar::typeNumber)
                            {
                                strCallStack+=varName;
                                strCallStack+="=";
                                strCallStack+=curVar->toStdString();
                                strCallStack+=";";
                            }
                            else if(curVar->getType()==AKVar::typeString)
                            {
                                strCallStack+=varName;
                                strCallStack+="=\"";
                                std::string curVal=curVar->toStdString();
                                AKCommon::str_c_escape(curVal);
                                strCallStack+=curVal;
                                strCallStack+="\";";
                            }
                        }
                    }
                }
                else
                {
                }
                /////////////////////////////////////////////////////////////////////////////////////
            }
            ////////////////////////////////////////////////////
            /*uint32 coreID=getID();
            sprintf(tmp,"%lu",coreID);
            info+="core:";
            info+=tmp;
            info+="\r\n";*/
            ////////////////////////////////////////////////////
            if(info!="")info+="|";
            info+=strCallStack;
            ////////////////////////////////////////////////////
            ++it;
        }
        return info;
    }

}
