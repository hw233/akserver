//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSERVER_H
#define AKSERVER_AKSERVER_H

#include "AKClientProxy.h"

namespace ak
{
    namespace net
    {
#define SERVER_SEND_BUF_BLOCK_COUNT 128//一次发送buf数据块最大数量
#define SERVER_SEND_DT 1//发送时间间隔（毫秒）

        //服务端类（类中所有方法均在同一物理线程中调用）
        class AKServer
        {
        public:
            AKServer(
                    AKSystem *system,//系统对象
                    AKCore *core,//核对象
                    AKProcess *process,//进程对象
                    uint32 port,//端口
                    std::string bindIP="",//监听绑定ip
                    uint32 clientProxyMaxCount=100,//客户端代理对象最大数量
                    uint32 keepActivedAcceptCount=10,//保持异步接受连接数量
                    uint32 recvBufSize=1024*10//接收缓冲尺寸
            );

            ~AKServer();

            std::string getClientIP(uint32 clientID);//获取client的IP
            void getActiveClientIDArr(AKUint32Array &arr);//获取活动client的id数组
            void closeClient(uint32 clientID);//关闭client
            AKClientProxy *getLost();//获取断线的client代理对象
            bool pop(uint32 clientID,uint32 popSize);//弹出接收数据
            const int8 *getRecvData(uint32 clientID);//获取接收数据
            AKClientProxy *getAcceptedClientProxy();//获取已接受连接的client代理对象
            uint32 getStatus();//获取状态:0=未启动;1=正在启动;2=启动成功;
            void start();//启动
            void close();//关闭
            void reqRecv(uint32 clientID);//请求接收数据
            uint32 getRecvSize(uint32 clientID);//获取接收数据尺寸
            uint32 hasRecv();//返回接收到数据的clientID

            void send(//发送多个buf
                    uint32 clientID,
                    container::AKBuf **bufArray,//数据buf数组
                    uint32 bufCount//数据buf数量
            );

            virtual void onStart(bool isSucc);//启动回调
            virtual void onAccept(AKClientProxy *proxy,boost::system::error_code error);//接受连接回调
            virtual void onLost(AKClientProxy *proxy);//连接断线回调
            virtual void onRecv(AKClientProxy *proxy);//接收数据回调
            virtual void onSend(AKClientProxy *proxy,bool isSendComplete);//发送数据回调

        private:
            void asynStart();
            void asynClose();
            void addAnyAsynAccept();
            void checkSend();

        private:
            uint32 mStatus;//状态:0=未启动;1=正在启动;2=启动成功;3=关闭中;
            boost::asio::ip::tcp::acceptor mAcceptor;

            std::string mBindIP;//监听绑定ip
            uint32 mPort;
            uint32 mClientProxyMaxCount;//client代理对象最大数量
            uint32 mKeepActivedAcceptCount;//保持等待连接的client代理对象数量
            uint32 mAsynAcceptCount;//当前保持等待连接的client代理对象数量
            AKClientProxy **mClientProxyArray;//client代理对象数组
            AKClientProxy **mFreeClientProxyArray;//空闲client代理对象数组
            uint32 mFreeClientProxyCount;//空闲client代理对象数量
            uint32 mRecvBufSize;//接收缓冲尺寸

            bool mIsDeleted;//是否被删除
            boost::asio::io_service &mIO;
            AKSystem *mSystem;
            AKCore *mCore;
            AKProcess *mProcess;
            boost::shared_ptr<boost::asio::deadline_timer> mSendTimerPtr;//发送定时器

            AKClientProxy **mAcceptedClientProxyArray;//已接受连接的client代理对象数组
            uint32 mAcceptedClientProxyCount;

            AKClientProxyMap mActiveClientProxyMap;//活动client代理对象map
            AKClientProxyMap mRecvClientProxyMap;//接收到数据的client代理对象map
            AKClientProxyList mRecvClientProxyList;//接收到数据的client代理对象list

            AKClientProxyList mLostClientProxyList;//断线list

            AKClientProxyMap mSendClientProxyMap;//发送数据的client代理对象map

        public:
            bool mIsStartComplete;
            int8 mSendTmpBuf[AKBUF_BLOCK_SIZE*SERVER_SEND_BUF_BLOCK_COUNT];//发送临时缓冲
        };
    }
}

#endif //AKSERVER_AKSERVER_H
