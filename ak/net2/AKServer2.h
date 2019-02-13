//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSERVER2_H
#define AKSERVER_AKSERVER2_H

namespace ak
{
    namespace net2
    {
        class AKServerSession;
        class AKServer2
        {
        public:
            typedef std::list<AKServerSession *> SessionList;
        public:
            AKServer2(boost::asio::io_service& io,uint16 port,ak::AKCore *core);
            AKServer2(boost::asio::io_service& io,const int8 *domain,uint16 port,ak::AKCore *core);
            virtual void onConnect(AKServerSession *session)=0;
            virtual void onData(AKServerSession *session,int8 *data,uint32 bytes)=0;
            virtual void onDisconnect(AKServerSession *session)=0;
            SessionList *getSessionList();
            void close();//关闭服务器
            void start();//启动服务器

        private:
            void new_accept();
            void handle_accept(AKServerSession* new_session,const boost::system::error_code& error);

        public:
            ak::AKCore *core;
        private:
            boost::asio::io_service& io;
            boost::asio::ip::tcp::acceptor acceptor;
            SessionList sessionList;
            bool startSign;
        };

        class AKServerSession
        {
        public:
            AKServerSession(AKServer2 &server,boost::asio::io_service& io);
            ~AKServerSession();
            boost::asio::ip::tcp::socket& socket();
            void start();
            void send(const int8 *data,uint32 bytes);
            void send(ak::container::AKBuf &buf);
            void close();
            std::string ip();

        private:
            void continueRecv();
            void continueSend();
            void handle_read(const boost::system::error_code& error,size_t bytes_transferred);
            void handle_write(const boost::system::error_code& error);
            void deleteThis();
            std::string _getIP();

        public:
            AKServer2::SessionList::iterator listPos;//在列表中的位置
            typedef std::list<ak::container::AKBuf *> SendList;
            SendList sendList;
        private:
            boost::asio::ip::tcp::socket sock;
            enum
            {
                sendBufBytes=8*1024,
                recvBufBytes=8*1024
            };
            int8 sendBuf[sendBufBytes];
            int8 recvBuf[recvBufBytes];
            bool connectSign;
            bool sendSign;
            bool recvSign;
            AKServer2 &server;
            std::string _ip;
        };
    }
}

#endif //AKSERVER_AKSERVER2_H
