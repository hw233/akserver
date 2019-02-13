//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKCLIENT_H
#define AKSERVER_AKCLIENT_H

namespace ak
{
    namespace net
    {
#define CLIENT_SEND_BUF_BLOCK_COUNT 128//一次发送buf数据块最大数量
#define CLIENT_SEND_DT 1//发送时间间隔（毫秒）

        //客户端类（类中所有方法均在同一物理线程中调用）
        class AKClient
        {
        public:
            AKClient(
                    AKSystem *system,//系统对象
                    AKCore *core,//核对象
                    AKProcess *process,//进程对象
                    const int8 *domain,//域名或ip
                    uint32 port,//端口
                    uint32 recvBufSize=1024*1024//接收缓冲尺寸
            );
            virtual ~AKClient();

            std::string ip();//获取IP

            void start();//启动
            void close();//关闭
            void reqRecv();//请求接收

            void send(//发送数据
                    const int8 *data,//数据
                    uint32 size//数据尺寸
            );
            void send(//发送多个数据
                    AKDataDesc *descArray,//数据描述数组
                    uint32 descCount//数据描述数量
            );
            void send(//发送数据buf
                    container::AKBuf &buf
            );
            void send(//发送多个buf
                    container::AKBuf **bufArray,//数据buf数组
                    uint32 bufCount//数据buf数量
            );

            bool hasCnn(bool &isSucc);//判断是否存在连接事件
            bool hasLost();////判断是否存在断线事件
            uint32 getStatus();//获取状态:0=未启动;1=正在启动;2=启动成功;
            bool hasRecv();//是否有收到新的数据
            uint32 getRecvSize();//获取接收数据尺寸
            bool pop(uint32 popSize);//弹出接收缓冲中的数据
            const int8 *getRecvData();//返回接收缓冲中未处理的数据

            virtual void onStart(bool isSucc);//启动回调
            virtual void onConnect(const boost::system::error_code& error);//连接回调
            virtual void onSend(const boost::system::error_code &error,const uint32 uSendSize);//发送回调
            virtual void onRecv(const boost::system::error_code &error,const uint32 uRecvSize);//接收回调
            //virtual uint32 onMsg(//处理网络消息(如果准备处理的数据还没接收完成,可以直接返回0,否则返回处理完的缓冲字节数)
            //	void *data,//准备处理的数据
            //	uint32 size//数据尺寸
            //	)=0;
            virtual void onClose();//关闭或断线回调

        private:
            void asynStart();
            void asynClose();
            void checkSend();

        protected:
            boost::asio::io_service &mIO;
            AKSystem *mSystem;
            AKCore *mCore;
            AKProcess *mProcess;

        private:
            uint32 mStatus;//状态:0=未启动;1=正在启动;2=启动成功;
            boost::asio::ip::tcp::socket *mSocket;
            boost::asio::ip::tcp::endpoint mEndPoint;

            boost::timer mSendLastTimer;//发送计时器
            boost::timer mRecvLastTimer;//发送计时器

            uint32 mRecvBufSize;//接收缓冲尺寸
            uint32 mRecvBufCurSize;//接收缓冲当前使用的尺寸
            int8 *mRecvBuf;//接收缓冲
            uint32 mProcessSize;//已处理数据的尺寸
            bool mHasRecv;//是否有收到数据
            bool mIsReqRecv;//请求接收标记
            ////////////////////////////////////////////////////////////////////
            bool mHasCnn;//是否存在连接事件
            bool mHasCnnSucc;//连接事件是否成功
            ////////////////////////////////////////////////////////////////////
            bool mHasLost;//是否存在断线事件
            ////////////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////////////

        public:
            ////////////////////////////////////////////////////////////////////
            bool mIsDeleted;//是否被删除
            bool mIsSendComplete;//是否发送完成
            container::AKBuf mSendBuf;//发送buf
            boost::shared_ptr<boost::asio::deadline_timer> mSendTimerPtr;//发送定时器
            ////////////////////////////////////////////////////////////////////
            std::string mDomain;//访问域名或ip
            uint32 mPort;//访问端口
            ////////////////////////////////////////////////////////////////////
            bool mIsConnectComplete;
            int8 mSendTmpBuf[AKBUF_BLOCK_SIZE*CLIENT_SEND_BUF_BLOCK_COUNT];//发送临时缓冲
            ////////////////////////////////////////////////////////////////////
        };
    }
}

#endif //AKSERVER_AKCLIENT_H
