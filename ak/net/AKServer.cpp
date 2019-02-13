#include "ak.h"
#include "AKServer.h"

namespace ak
{
    namespace net
    {
        AKServer::AKServer(
                AKSystem *system,//系统对象
                AKCore *core,//核对象
                AKProcess *process,//进程对象
                uint32 port,//端口
                std::string bindIP,//监听绑定ip
                uint32 clientProxyMaxCount,//客户端代理对象最大数量
                uint32 keepActivedAcceptCount,//保持异步接受连接数量
                uint32 recvBufSize//接收缓冲尺寸
        )
                :mIsDeleted(false),
                 mSystem(system),
                 mCore(core),
                 mProcess(process),
                 mIO(core->getIO()),
                 mAcceptor(core->getIO()),
                 mStatus(0),
                 mPort(port),
                 mBindIP(bindIP),
                 mClientProxyMaxCount(clientProxyMaxCount),
                 mKeepActivedAcceptCount(keepActivedAcceptCount),
                 mFreeClientProxyCount(0),
                 mAsynAcceptCount(0),
                 mRecvBufSize(recvBufSize),
                 mIsStartComplete(false),
                 mAcceptedClientProxyCount(0)
        {
            //已接受连接数组
            mAcceptedClientProxyArray=new AKClientProxy *[mClientProxyMaxCount];

            //空闲数组
            mFreeClientProxyArray=new AKClientProxy *[mClientProxyMaxCount];

            //创建client代理对象
            mClientProxyArray=new AKClientProxy *[mClientProxyMaxCount+1];
            mClientProxyArray[0]=NULL;
            for(uint32 i=1;i<=mClientProxyMaxCount;++i)
            {
                AKClientProxy *proxy=new AKClientProxy(i,mSystem,mCore,mProcess,*this,mRecvBufSize);
                mClientProxyArray[i]=proxy;

                //空闲client代理对象
                mFreeClientProxyArray[mFreeClientProxyCount]=proxy;
                ++mFreeClientProxyCount;
            }

            //定时调用
            mSendTimerPtr=boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(mIO,boost::posix_time::millisec(SERVER_SEND_DT)));
            mSendTimerPtr->async_wait(boost::bind(&AKServer::checkSend,this));
        }

        AKServer::~AKServer()
        {
            for(uint32 i=1;i<=mClientProxyMaxCount;++i)
            {
                AKClientProxy *proxy=mClientProxyArray[i];
                delete proxy;
            }
            delete [] mClientProxyArray;
            delete [] mFreeClientProxyArray;
            delete [] mAcceptedClientProxyArray;
        }

        uint32 AKServer::getStatus()
        {
            return mStatus;
        }

        std::string AKServer::getClientIP(uint32 clientID)
        {
            if(clientID>mClientProxyMaxCount)return "";
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            if(mActiveClientProxyMap.find(proxy)!=mActiveClientProxyMap.end())//活动的client代理对象
            {
                return proxy->ip();
            }
            return "";
        }

        void AKServer::getActiveClientIDArr(AKUint32Array &arr)
        {
            AKClientProxyMap::iterator it=mActiveClientProxyMap.begin();
            while(it!=mActiveClientProxyMap.end())
            {
                net::AKClientProxy *proxy=it->first;
                arr.push_back(proxy->getID());
                ++it;
            }
        }

        void AKServer::closeClient(uint32 clientID)
        {
            if(clientID>mClientProxyMaxCount)return;
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            if(mActiveClientProxyMap.find(proxy)!=mActiveClientProxyMap.end())//活动的client代理对象
            {
                return proxy->close();
            }
        }

        AKClientProxy *AKServer::getLost()
        {
            if(mLostClientProxyList.size()==0)return NULL;
            AKClientProxyList::iterator it=mLostClientProxyList.begin();
            AKClientProxy *proxy=*it;
            mLostClientProxyList.erase(it);
            return proxy;
        }

        bool AKServer::pop(uint32 clientID,uint32 popSize)
        {
            if(clientID>mClientProxyMaxCount)return false;
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            return proxy->pop(popSize);
        }

        const int8 *AKServer::getRecvData(uint32 clientID)
        {
            if(clientID>mClientProxyMaxCount)return NULL;
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            return proxy->getRecvData();
        }

        uint32 AKServer::hasRecv()
        {
            if(mRecvClientProxyList.size()==0)return 0;
            AKClientProxyList::iterator it=mRecvClientProxyList.begin();
            AKClientProxy *proxy=*it;
            mRecvClientProxyList.erase(it);
            mRecvClientProxyMap.erase(proxy);
            return proxy->getID();
        }

        uint32 AKServer::getRecvSize(uint32 clientID)
        {
            if(clientID>mClientProxyMaxCount)return 0;
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            if(mActiveClientProxyMap.find(proxy)!=mActiveClientProxyMap.end())//活动的client代理对象
            {
                return proxy->getRecvSize();
            }
            return 0;
        }

        void AKServer::reqRecv(uint32 clientID)
        {
            if(clientID>mClientProxyMaxCount)return;
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            if(mActiveClientProxyMap.find(proxy)!=mActiveClientProxyMap.end())//活动的client代理对象
            {
                proxy->reqRecv();
            }
        }

        void AKServer::close()
        {
            mIO.post(boost::bind(&AKServer::asynClose,this));
        }

        void AKServer::asynClose()
        {
            if(mStatus==2)//启动成功
            {
                mStatus=3;//关闭中
                mAcceptor.close();
            }
        }

        void AKServer::start()
        {
            mIO.post(boost::bind(&AKServer::asynStart,this));
        }

        void AKServer::asynStart()
        {
            if(mStatus!=0)return;//已启动或正在启动

            mStatus=1;//正在启动
            mAcceptedClientProxyCount=0;//清除已接受连接数组
            mLostClientProxyList.clear();//清除断线数组
            mRecvClientProxyMap.clear();//清除接收数据容器
            mRecvClientProxyList.clear();//清除接收数据容器
            mSendClientProxyMap.clear();//清除发送数据容器

            //设置IP地址和端口
            boost::asio::ip::tcp::endpoint endPoint;
            if(mBindIP=="")endPoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),mPort);
            else endPoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(mBindIP),mPort);

            //打开endPoint指定的协议(即TCP协议)
            mAcceptor.open(endPoint.protocol());

            //设置允许复用一个正在使用的IP地址
            mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

            //绑定IP地址和端口
            boost::system::error_code error;
            mAcceptor.bind(endPoint,error);
            if(error)
            {
                //启动失败
                mAcceptor.close();
                mIO.post(boost::bind(&AKServer::onStart,this,false));
                mStatus=0;//未启动
                return;
            }

            //监听
            mAcceptor.listen();
            mStatus=2;//启动成功

            //补充投递一些异步接受连接请求
            addAnyAsynAccept();

            //启动成功
            mIO.post(boost::bind(&AKServer::onStart,this,true));
        }

        void AKServer::addAnyAsynAccept()
        {
            if(mStatus!=2)return;//未启动

            if(mAsynAcceptCount<mKeepActivedAcceptCount)
            {
                //如果投递的异步接受连接请求数太少，再投递一些
                uint32 c=0;
                uint32 needCount=mKeepActivedAcceptCount-mAsynAcceptCount;
                if(needCount<=mFreeClientProxyCount)c=needCount;
                else c=mFreeClientProxyCount;

                for(uint32 i=0;i<c;++i)
                {
                    AKClientProxy *proxy=mFreeClientProxyArray[mFreeClientProxyCount-1];
                    proxy->reset();
                    mAcceptor.async_accept(
                            proxy->getSocket(),
                            boost::bind(&AKServer::onAccept,this,proxy,boost::asio::placeholders::error)
                    );
                    --mFreeClientProxyCount;
                    ++mAsynAcceptCount;
                }
            }
        }

        void AKServer::onStart(bool isSucc)
        {
            mIsStartComplete=true;
        }

        AKClientProxy *AKServer::getAcceptedClientProxy()
        {
            if(mAcceptedClientProxyCount==0)return NULL;
            --mAcceptedClientProxyCount;
            return mAcceptedClientProxyArray[mAcceptedClientProxyCount];
        }

        void AKServer::onAccept(AKClientProxy *proxy,boost::system::error_code error)
        {
            --mAsynAcceptCount;

            if(error)//接受连接失败
            {
                //回收空闲client代理对象
                mFreeClientProxyArray[mFreeClientProxyCount]=proxy;
                ++mFreeClientProxyCount;
            }
            else
            {
                mAcceptedClientProxyArray[mAcceptedClientProxyCount]=proxy;
                ++mAcceptedClientProxyCount;

                //添加到活动map
                mActiveClientProxyMap[proxy]=proxy;
            }

            if(mStatus==3)//关闭中
            {
                if(mAsynAcceptCount==0)
                {
                    mStatus=0;//未启动
                }
            }
            else
            {
                //补充投递一些异步接受连接请求
                addAnyAsynAccept();
            }
        }

        void AKServer::onLost(AKClientProxy *proxy)
        {
            proxy->reset();

            //回收空闲client代理对象
            mFreeClientProxyArray[mFreeClientProxyCount]=proxy;
            ++mFreeClientProxyCount;

            //从活动map中删除
            mActiveClientProxyMap.erase(proxy);

            //添加到断线list
            mLostClientProxyList.push_back(proxy);

            //补充投递一些异步接受连接请求
            addAnyAsynAccept();
        }

        void AKServer::onRecv(AKClientProxy *proxy)
        {
            if(mRecvClientProxyMap.find(proxy)==mRecvClientProxyMap.end())//未存在
            {
                mRecvClientProxyList.push_back(proxy);
                mRecvClientProxyMap[proxy]=proxy;
            }
        }

        void AKServer::send(//发送多个buf
                uint32 clientID,
                container::AKBuf **bufArray,//数据buf数组
                uint32 bufCount//数据buf数量
        )
        {
            if(clientID>mClientProxyMaxCount)return;
            net::AKClientProxy *proxy=mClientProxyArray[clientID];
            if(mActiveClientProxyMap.find(proxy)!=mActiveClientProxyMap.end())//活动的client代理对象
            {
                proxy->send(bufArray,bufCount);
            }
        }

        void AKServer::checkSend()
        {
            if(mCore->isQuit()||//系统退出时
               mIsDeleted//被删除时
                    )
            {
                asynClose();
                return;
            }

            //发送数据
            AKClientProxyMap::iterator it=mSendClientProxyMap.begin();
            while(it!=mSendClientProxyMap.end())
            {
                AKClientProxy *proxy=it->second;
                ++it;
                proxy->checkSend();
            }

            if(!mCore->isQuit())
            {
                //mSendTimerPtr->expires_from_now(boost::posix_time::millisec(SERVER_SEND_DT));//重新设置定时器
                //mSendTimerPtr->async_wait(boost::bind(&AKServer::checkSend,this));//定时调用
                mIO.post(boost::bind(&AKServer::checkSend,this));
            }
        }

        void AKServer::onSend(AKClientProxy *proxy,bool isSendComplete)
        {
            if(isSendComplete)
            {
                mSendClientProxyMap.erase(proxy);
            }
            else
            {
                mSendClientProxyMap[proxy]=proxy;
            }
        }

    }
}
