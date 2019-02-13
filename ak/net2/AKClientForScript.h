//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKCLIENTFORSCRIPT_H
#define AKSERVER_AKCLIENTFORSCRIPT_H

namespace ak
{
    namespace net2
    {
        class AKClientForScript:public ak::net2::AKClient2
        {
        public:
            AKClientForScript(boost::asio::io_service &io,ak::AKCore *core);
            void onConnect(bool isSucc);
            void onData(int8 *data,uint32 bytes);
            void onDisconnect();

        public:
            bool connectSign;//连接成功标记
            bool recvSign;//收到数据标记
            bool netErrorSign;//网络异常标记（包含连接失败和断开连接）
            ak::container::AKBuf recvBuf;//接受缓冲
        };
    }
}

#endif //AKSERVER_AKCLIENTFORSCRIPT_H
