#include "ak.h"
#include "AKClientForScript.h"

namespace ak
{
    namespace net2
    {
        AKClientForScript::AKClientForScript(boost::asio::io_service &io,ak::AKCore *core)
                :AKClient2(io,core),
                 recvBuf(core),
                 connectSign(false),
                 recvSign(false),
                 netErrorSign(false)
        {
        }

        void AKClientForScript::onConnect(bool isSucc)
        {
            if(isSucc)
            {
                connectSign=true;
            }
            else
            {
                netErrorSign=true;
            }
        }

        void AKClientForScript::onDisconnect()
        {
            netErrorSign=true;
        }

        void AKClientForScript::onData(int8 *data,uint32 bytes)
        {
            recvSign=true;
            recvBuf.setEnd();
            recvBuf.write(data,bytes);
        }
    }
}