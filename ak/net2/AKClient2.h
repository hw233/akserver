//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKCLIENT2_H
#define AKSERVER_AKCLIENT2_H

namespace ak
{
    namespace net2
    {
        class AKClient2
        {
        public:
            AKClient2(boost::asio::io_service &io,ak::AKCore *core);
            ~AKClient2();

            virtual void onConnect(bool isSucc)=0;
            virtual void onData(int8 *data,uint32 bytes)=0;
            virtual void onDisconnect()=0;

            void connect(const int8 *ip,uint16 port);
            void send(const int8 *data,uint32 bytes);
            void send(ak::container::AKBuf &buf);
            void close();
            static std::string domain2IP(const int8 *domain);
        private:
            void handle_connect(const boost::system::error_code& error);
            void handle_connect2(bool error);
            void continueRecv();
            void continueSend();
            void handle_read(const boost::system::error_code& error,size_t bytes_transferred);
            void handle_write(const boost::system::error_code& error);
            void tryClose();
        private:
            boost::asio::io_service &io;
            uint32 connectSign;//连接标记：0=未连接;1=正在连接;2=连接正常;
            boost::asio::ip::tcp::socket sock;
            enum
            {
                sendBufBytes=8*1024,
                recvBufBytes=8*1024
            };
            int8 sendBuf[sendBufBytes];
            int8 recvBuf[recvBufBytes];
            bool sendSign;
            bool recvSign;
            typedef std::list<ak::container::AKBuf *> SendList;
            SendList sendList;
            ak::AKCore *core;
        };
    }
}

#endif //AKSERVER_AKCLIENT2_H
