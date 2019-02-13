//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKCLIENTPROXY_H
#define AKSERVER_AKCLIENTPROXY_H

namespace ak
{
    namespace net
    {
        class AKServer;

        //客户端代理类（类中所有方法均在同一物理线程中调用）
        class AKClientProxy
        {
        public:
            AKClientProxy(
                    uint32 id,//clientID
                    AKSystem *system,//系统对象
                    AKCore *core,//核对象
                    AKProcess *process,//进程对象
                    AKServer &server,//所属服务器
                    uint32 recvBufSize//接收缓冲大小
            );

            ~AKClientProxy();

            uint32 getID();
            std::string ip();//获取IP
            void reset();
            boost::asio::ip::tcp::socket &getSocket();
            void reqRecv();//请求接收数据
            void close();

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

            void checkSend();

            bool hasRecv();//是否有收到新的数据
            uint32 getRecvSize();//获取接收数据尺寸
            bool pop(uint32 popSize);//弹出接收缓冲中的数据
            const int8 *getRecvData();//返回接收缓冲中未处理的数据

            virtual void onRecv(const boost::system::error_code &error,const uint32 uRecvSize);//接收回调
            virtual void onClose();//关闭或断线回调
            virtual void onSend(const boost::system::error_code &error,const uint32 uSendSize);//发送完成回调

        private:
            uint32 mID;
            //uint32 mStatus;//状态:0=未启动;1=正在启动;2=启动成功;

            boost::asio::io_service &mIO;
            AKSystem *mSystem;
            AKCore *mCore;
            AKProcess *mProcess;
            AKServer &mServer;
            boost::asio::ip::tcp::socket *mSocket;

            boost::timer mSendLastTimer;//发送计时器
            boost::timer mRecvLastTimer;//发送计时器

            uint32 mRecvBufSize;//接收缓冲尺寸
            uint32 mRecvBufCurSize;//接收缓冲当前使用的尺寸
            int8 *mRecvBuf;//接收缓冲
            uint32 mProcessSize;//已处理数据的尺寸
            bool mHasRecv;//是否有收到数据
            bool mIsReqRecv;//请求接收标记

            bool mIsSendComplete;//是否发送完成
            container::AKBuf mSendBuf;//发送buf
        };
    }
}

#endif //AKSERVER_AKCLIENTPROXY_H
