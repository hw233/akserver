//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSERVERFORSCRIPT_H
#define AKSERVER_AKSERVERFORSCRIPT_H

namespace ak
{
    namespace net2
    {
        class AKServerForScript:public ak::net2::AKServer2
        {
        public:
            struct SessionInfo
            {
            public:
                SessionInfo(uint32 id,ak::net2::AKServerSession *session,ak::AKCore *core);
                uint32 id;
                ak::net2::AKServerSession *session;
                ak::container::AKBuf recvBuf;
            };
            typedef std::map<ak::net2::AKServerSession *,SessionInfo *> SessionMap;
            typedef std::map<uint32,uint32> Uint32Uint32Map;
            typedef std::map<uint32,ak::net2::AKServerSession *> SessionIDMap;
            AKServerForScript(
                    boost::asio::io_service &io,
                    const int8 *bindDomain,//绑定的域名或端口
                    uint16 port,//端口
                    ak::AKCore *core
            );
            AKServerForScript(
                    boost::asio::io_service &io,
                    uint16 port,//端口
                    ak::AKCore *core
            );
            ~AKServerForScript();
            void onConnect(ak::net2::AKServerSession *session);
            void onDisconnect(ak::net2::AKServerSession *session);
            void onData(ak::net2::AKServerSession *session,int8 *data,uint32 bytes);

        public:
            SessionMap sessionMap;//会话列表
            SessionIDMap sessionIDMap;
            Uint32Uint32Map recvSessionIDMap;//收到数据的sessionID列表
            Uint32Uint32Map connectSessionIDMap;//连接的sessionID列表
            Uint32Uint32Map disconnectSessionIDMap;//断开连接的sessionID列表
            uint32 sessionID;
        };
    }
}

#endif //AKSERVER_AKSERVERFORSCRIPT_H
