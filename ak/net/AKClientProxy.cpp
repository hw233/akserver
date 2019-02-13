#include "ak.h"
#include "AKClientProxy.h"

namespace ak
{
    namespace net
    {
        AKClientProxy::AKClientProxy(
                uint32 id,//clientID
                AKSystem *system,//系统对象
                AKCore *core,//核对象
                AKProcess *process,//进程对象
                AKServer &server,//所属服务器
                uint32 recvBufSize//接收缓冲大小
        )
                :mID(id),
                 mSystem(system),
                 mCore(core),
                 mProcess(process),
                 mIO(core->getIO()),
                 mServer(server),
                 mProcessSize(0),
                 mRecvBufCurSize(0),
                 mRecvBufSize(recvBufSize),
                 mIsSendComplete(true),
                 mHasRecv(false),
                 mSendBuf(core),
                 mIsReqRecv(false)
        {
            mSocket=new boost::asio::ip::tcp::socket(mIO);
            mRecvBuf=new int8[mRecvBufSize];
        }

        AKClientProxy::~AKClientProxy()
        {
            delete mSocket;
            delete [] mRecvBuf;
        }

        std::string AKClientProxy::ip()
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

        void AKClientProxy::reset()
        {
            //清理
            mSendLastTimer.restart();
            mRecvLastTimer.restart();
            mRecvBufCurSize=0;
            mProcessSize=0;
            mIsSendComplete=true;
            mSendBuf.clear();
        }

        uint32 AKClientProxy::getID()
        {
            return mID;
        }

        boost::asio::ip::tcp::socket &AKClientProxy::getSocket()
        {
            return *mSocket;
        }

        void AKClientProxy::close()
        {
            reqRecv();//调用close之前必须调用reqRecv才能触发关闭回调
            mSocket->close();
        }

        uint32 AKClientProxy::getRecvSize()
        {
            if(mHasRecv)
            {
                return (mRecvBufCurSize-mProcessSize);
            }
            return 0;
        }

        bool AKClientProxy::hasRecv()
        {
            return mHasRecv;
        }

        const int8 *AKClientProxy::getRecvData()
        {
            return &mRecvBuf[mProcessSize];
        }

        bool AKClientProxy::pop(uint32 popSize)
        {
            if((mProcessSize+popSize)>mRecvBufCurSize)return false;
            mProcessSize+=popSize;
            return true;
        }

        void AKClientProxy::reqRecv()
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
                    boost::bind(&AKClientProxy::onRecv,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
            );
        }

        void AKClientProxy::onRecv(const boost::system::error_code &error,const uint32 recvSize)
        {
            mIsReqRecv=false;
            mRecvLastTimer.restart();

            if(error||recvSize==0)//断线
            {
                onClose();
                return;
            }

            mRecvBufCurSize+=recvSize;
            mHasRecv=true;//标记收到新数据

            mServer.onRecv(this);
        }

        void AKClientProxy::onClose()
        {
            mSocket->close();
            mServer.onLost(this);
        }

        void AKClientProxy::send(const int8 *data,uint32 size)
        {
            mSendBuf.write(data,size);
            mServer.onSend(this,false);
        }

        void AKClientProxy::send(AKDataDesc *descArray,uint32 descCount)
        {
            for(uint32 i=0;i<descCount;++i)
            {
                mSendBuf.write((const int8 *)descArray[i].data,descArray[i].size);
            }
            mServer.onSend(this,false);
        }

        void AKClientProxy::send(container::AKBuf &buf)
        {
            mSendBuf.append(buf);
            mServer.onSend(this,false);
        }

        void AKClientProxy::send(container::AKBuf **bufArray,uint32 bufCount)
        {
            for(uint32 i=0;i<bufCount;++i)
            {
                mSendBuf.append(*bufArray[i]);
            }
            mServer.onSend(this,false);
        }

        void AKClientProxy::onSend(const boost::system::error_code &error,const uint32 uSendSize)
        {
            mIsSendComplete=true;
            if(error)
            {
                mSocket->close();
            }
        }

        void AKClientProxy::checkSend()
        {
            if(mIsSendComplete)//之前请求的数据发送完成
            {
                if(mSendBuf.size()>0)//还有要发送的数据
                {
                    mIsSendComplete=false;

                    //打包发送数据缓冲
                    uint32 readBytes=0;
                    uint32 readCount=mSendBuf.getBlockCount();
                    if(readCount>SERVER_SEND_BUF_BLOCK_COUNT)readCount=SERVER_SEND_BUF_BLOCK_COUNT;//设置发送数据块数量
                    int8 *sendTmpBuf=mServer.mSendTmpBuf;//一次性发送buf
                    for(uint32 i=0;i<readCount;++i)
                    {
                        uint32 blockSize=0;
                        const int8 *data=mSendBuf.getBlockData(0,blockSize);
                        memcpy(&sendTmpBuf[readBytes],data,blockSize);//放入一次性发送buf
                        readBytes+=blockSize;
                        mSendBuf.popBlock();
                    }

                    ////投递发送请求//async_write_some不保证完整发完
                    //mSocket->async_write_some(
                    //	boost::asio::buffer(data,blockSize),
                    //	boost::bind(&AKClientProxy::onSend,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
                    //	);

                    //投递发送请求
                    boost::asio::async_write(
                            *mSocket,
                            boost::asio::buffer(sendTmpBuf,readBytes),
                            boost::bind(&AKClientProxy::onSend,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
                    );
                }
                else//所有数据发送完成
                {
                    mServer.onSend(this,true);
                }
            }
        }

    }

}
