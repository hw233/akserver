#include "ak.h"
#include "AKClient.h"

namespace ak
{
    namespace net
    {
        AKClient::AKClient(
                AKSystem *system,//系统对象
                AKCore *core,//核对象
                AKProcess *process,//进程对象
                const int8 *domain,
                uint32 port,
                uint32 recvBufSize
        )
                :mIsDeleted(false),
                 mSystem(system),
                 mCore(core),
                 mProcess(process),
                 mIO(core->getIO()),
                 mStatus(0),//未启动
                 mDomain(domain),
                 mPort(port),
                 mProcessSize(0),
                 mRecvBufCurSize(0),
                 mRecvBufSize(recvBufSize),
                 mIsSendComplete(true),
                 mHasRecv(false),
                 mIsConnectComplete(false),
                 mSendBuf(core),
                 mIsReqRecv(false),
                 mHasCnn(false),
                 mHasCnnSucc(false),
                 mHasLost(false)
        {
            mSocket=new boost::asio::ip::tcp::socket(mIO);
            mRecvBuf=new int8[mRecvBufSize];

            //定时调用
            mSendTimerPtr=boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(mIO,boost::posix_time::millisec(CLIENT_SEND_DT)));
            mSendTimerPtr->async_wait(boost::bind(&AKClient::checkSend,this));
        }

        AKClient::~AKClient()
        {
            delete mSocket;
            delete [] mRecvBuf;
        }

        bool AKClient::hasLost()
        {
            if(mHasLost)
            {
                mHasLost=false;
                return true;
            }
            return false;
        }

        bool AKClient::hasCnn(bool &isSucc)
        {
            isSucc=mHasCnnSucc;
            if(mHasCnn)
            {
                mHasCnn=false;
                return true;
            }
            return false;
        }

        std::string AKClient::ip()
        {
            std::string ip="";
            try
            {
                ip=mSocket->remote_endpoint().address().to_string();
            }
            catch(...)
            {
            }
            return ip;
        }

        uint32 AKClient::getStatus()
        {
            return mStatus;
        }

        bool AKClient::hasRecv()
        {
            return mHasRecv;
        }

        const int8 *AKClient::getRecvData()
        {
            return &mRecvBuf[mProcessSize];
        }

        uint32 AKClient::getRecvSize()
        {
            if(mHasRecv)
            {
                return (mRecvBufCurSize-mProcessSize);
            }
            return 0;
        }

        void AKClient::close()
        {
            mIO.post(boost::bind(&AKClient::asynClose,this));
        }

        void AKClient::asynClose()
        {
            if(mStatus==0)return;//未启动
            reqRecv();//调用close之前必须调用reqRecv才能触发关闭回调
            mSocket->close();
        }

        void AKClient::onClose()
        {
            mHasLost=true;
        }

        void AKClient::start()
        {
            mIO.post(boost::bind(&AKClient::asynStart,this));
        }

        void AKClient::asynStart()
        {
            if(mStatus!=0)return;//正在启动或已经启动

            //域名转换成IP地址
            std::string ip=AKCommon::domain2IP(mDomain);
            if(ip=="")
            {
                mIO.post(boost::bind(&AKClient::onStart,this,false));
                return;
            }

            mHasRecv=false;//标记未收到新数据
            mStatus=1;//启动中
            mEndPoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ip),mPort);

            //清理
            mSendLastTimer.restart();
            mRecvLastTimer.restart();
            mRecvBufCurSize=0;
            mProcessSize=0;
            mIsSendComplete=true;
            mSendBuf.clear();

            mSocket->async_connect(mEndPoint,boost::bind(&AKClient::onConnect,this,boost::asio::placeholders::error));
        }

        void AKClient::onStart(bool isSucc)
        {
            mIsConnectComplete=true;
            mHasCnn=true;
            mHasCnnSucc=isSucc;
        }

        void AKClient::onConnect(const boost::system::error_code& error)
        {
            mIsConnectComplete=true;
            if(error)
            {
                mSocket->close();
                mStatus=0;//未启动
                onStart(false);
            }
            else
            {
                //请求接收
                //reqRecv();

                //设置开启心跳检查
                boost::system::error_code ec;
                boost::asio::socket_base::keep_alive option(true);
                mSocket->set_option(option,ec);

                mStatus=2;//启动成功
                onStart(true);
            }
        }

        bool AKClient::pop(uint32 popSize)
        {
            if((mProcessSize+popSize)>mRecvBufCurSize)return false;
            mProcessSize+=popSize;
            return true;
        }

        void AKClient::onRecv(const boost::system::error_code &error,const uint32 recvSize)
        {
            mIsReqRecv=false;
            mRecvLastTimer.restart();

            if(error||recvSize==0)//断线
            {
                mStatus=0;//未启动
                onClose();
                return;
            }

            mRecvBufCurSize+=recvSize;
            mHasRecv=true;//标记收到新数据
        }

        void AKClient::reqRecv()
        {
            if(mIsReqRecv)return;
            mIsReqRecv=true;

            if(mProcessSize>0)
            {
                //弹出处理完的数据
                memcpy(mRecvBuf,&mRecvBuf[mProcessSize],mRecvBufCurSize-mProcessSize);
                mRecvBufCurSize-=mProcessSize;
                mProcessSize=0;
            }

            mHasRecv=false;//标记未收到新数据

            //投递下一次接收数据的请求
            mSocket->async_read_some(
                    boost::asio::buffer(&mRecvBuf[mRecvBufCurSize],mRecvBufSize-mRecvBufCurSize),
                    boost::bind(&AKClient::onRecv,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
            );
        }

        void AKClient::send(const int8 *data,uint32 size)
        {
            if(mStatus!=2)
            {
                return;
            }
            mSendBuf.write(data,size);
        }

        void AKClient::send(AKDataDesc *descArray,uint32 descCount)
        {
            if(mStatus!=2)
            {
                return;
            }
            for(uint32 i=0;i<descCount;++i)
            {
                mSendBuf.write((const int8 *)descArray[i].data,descArray[i].size);
            }
        }

        void AKClient::send(container::AKBuf &buf)
        {
            if(mStatus!=2)
            {
                return;
            }
            mSendBuf.append(buf);
        }

        void AKClient::send(container::AKBuf **bufArray,uint32 bufCount)
        {
            if(mStatus!=2)
            {
                return;
            }
            for(uint32 i=0;i<bufCount;++i)
            {
                mSendBuf.append(*bufArray[i]);
            }
        }

        void AKClient::onSend(const boost::system::error_code &error,const uint32 uSendSize)
        {
            mIsSendComplete=true;
            if(error)
            {
                mSocket->close();
            }
        }

        void AKClient::checkSend()
        {
            if(mCore->isQuit())//系统退出时
            {
                asynClose();
                return;
            }
            if(mIsDeleted)//被删除时
            {
                delete this;
                return;
            }

            if(mStatus==2&&//启动成功
               mIsSendComplete&&//发送完成
               mSendBuf.size()>0
                    )
            {
                mIsSendComplete=false;

                //打包发送数据缓冲
                uint32 readBytes=0;
                uint32 readCount=mSendBuf.getBlockCount();
                if(readCount>CLIENT_SEND_BUF_BLOCK_COUNT)readCount=CLIENT_SEND_BUF_BLOCK_COUNT;//设置发送数据块数量
                int8 *sendTmpBuf=mSendTmpBuf;//一次性发送buf
                for(uint32 i=0;i<readCount;++i)
                {
                    uint32 blockSize=0;
                    const int8 *data=mSendBuf.getBlockData(0,blockSize);
                    memcpy(&sendTmpBuf[readBytes],data,blockSize);//放入一次性发送buf
                    readBytes+=blockSize;
                    mSendBuf.popBlock();
                }

                /*uint32 readBytes=mSendBuf.size();
                printf("%lu\n",readBytes);
                int8 *tmp=new int8[readBytes];
                mSendBuf.read(tmp,readBytes);
                mSendBuf.clear();*/

                //投递发送请求
                boost::asio::async_write(
                        *mSocket,
                        boost::asio::buffer(sendTmpBuf,readBytes),
                        //boost::asio::buffer(tmp,readBytes),
                        boost::bind(&AKClient::onSend,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
                );

                //delete [] tmp;
            }

            if(!mCore->isQuit())
            {
                //mSendTimerPtr->expires_from_now(boost::posix_time::millisec(CLIENT_SEND_DT));//重新设置定时器
                //mSendTimerPtr->async_wait(boost::bind(&AKClient::checkSend,this));//定时调用
                mIO.post(boost::bind(&AKClient::checkSend,this));
            }
        }
    }

}
