#include "ak.h"
#include "AKServerForScript.h"

namespace ak
{
    namespace net2
    {
        AKServerForScript::SessionInfo::SessionInfo(uint32 id,ak::net2::AKServerSession *session,ak::AKCore *core):
                id(id),
                session(session),
                recvBuf(core)
        {
        }

        AKServerForScript::AKServerForScript(
                boost::asio::io_service &io,
                const int8 *bindDomain,//绑定的域名或端口
                uint16 port,//端口
                ak::AKCore *core
        ):
                ak::net2::AKServer2(io,ak::AKCommon::domain2IP(bindDomain).c_str(),port,core),
                sessionID(0)
        {
        }
        AKServerForScript::AKServerForScript(
                boost::asio::io_service &io,
                uint16 port,//端口
                ak::AKCore *core
        ):
                ak::net2::AKServer2(io,port,core),
                sessionID(0)
        {
            start();
        }
        AKServerForScript::~AKServerForScript()
        {
            SessionMap::iterator it=sessionMap.begin();
            while(it!=sessionMap.end())
            {
                delete it->second;
                ++it;
            }
            sessionMap.clear();
            sessionIDMap.clear();
        }
        void AKServerForScript::onConnect(ak::net2::AKServerSession *session)
        {
            ++sessionID;
            if(sessionID==0)++sessionID;
            sessionMap[session]=new SessionInfo(sessionID,session,core);
            sessionIDMap[sessionID]=session;
            connectSessionIDMap[sessionID]=0;
        }
        void AKServerForScript::onDisconnect(ak::net2::AKServerSession *session)
        {
            SessionMap::iterator it=sessionMap.find(session);
            SessionInfo *info=it->second;
            sessionMap.erase(it);
            sessionIDMap.erase(sessionIDMap.find(info->id));
            disconnectSessionIDMap[info->id]=0;
            delete info;
        }
        void AKServerForScript::onData(ak::net2::AKServerSession *session,int8 *data,uint32 bytes)
        {
            SessionInfo *info=sessionMap[session];
            info->recvBuf.setEnd();
            info->recvBuf.write(data,bytes);
            recvSessionIDMap[info->id]=0;
        }
    }
}