#include "ak.h"
#include "AKServer2.h"

namespace ak
{
    namespace net2
    {
        class AKServerSession;

        AKServer2::AKServer2(boost::asio::io_service& io,uint16 port,ak::AKCore *core):
                io(io),
                acceptor(io,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)),
                startSign(false),
                core(core)
        {
        }

        AKServer2::AKServer2(boost::asio::io_service& io,const int8 *domain,uint16 port,ak::AKCore *core):
                io(io),
                acceptor(io,boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ak::AKCommon::domain2IP(domain)),port)),
                startSign(false),
                core(core)
        {
        }

        AKServer2::SessionList *AKServer2::getSessionList()
        {
            return &sessionList;
        }

        void AKServer2::start()
        {
            if(startSign)return;
            startSign=true;
            new_accept();
        }

        void AKServer2::close()
        {
            startSign=false;
            acceptor.close();

            SessionList::iterator it=sessionList.begin();
            while(it!=sessionList.end())
            {
                AKServerSession *session=*it;
                session->close();
                ++it;
            }
            sessionList.clear();
        }

        void AKServer2::new_accept()
        {
            AKServerSession* new_session=new AKServerSession(*this,io);
            acceptor.async_accept(
                    new_session->socket(),
                    boost::bind(&AKServer2::handle_accept,this,new_session,boost::asio::placeholders::error)
            );
        }

        void AKServer2::handle_accept(AKServerSession* new_session,const boost::system::error_code& error)
        {
            if(!error)
            {
                sessionList.push_back(new_session);
                new_session->listPos=sessionList.end();
                --new_session->listPos;
                new_session->start();
                new_accept();
            }
            else
            {
                delete new_session;
                startSign=false;
            }
        }

        AKServerSession::AKServerSession(AKServer2 &server,boost::asio::io_service& io):
                server(server),
                sock(io),
                connectSign(false),
                sendSign(false),
                recvSign(false)
        {
        }

        AKServerSession::~AKServerSession()
        {
            if(connectSign)
            {
                server.getSessionList()->erase(listPos);
                server.onDisconnect(this);
            }
            SendList::iterator it=sendList.begin();
            while(it!=sendList.end())
            {
                ak::container::AKBuf *buf=*it;
                delete buf;
                ++it;
            }
        }

        std::string AKServerSession::ip()
        {
            return _ip;
        }

        std::string AKServerSession::_getIP()
        {
            std::string ip="";
            try
            {
                ip=sock.remote_endpoint().address().to_string();
            }
            catch(...)
            {
            }
            return ip;
        }

        boost::asio::ip::tcp::socket& AKServerSession::socket()
        {
            return sock;
        }

        void AKServerSession::start()
        {
            connectSign=true;
            _ip=_getIP();
            server.onConnect(this);
            continueRecv();
        }

        void AKServerSession::send(const int8 *data,uint32 bytes)
        {
            if(bytes==0)return;
            ak::container::AKBuf *buf=new ak::container::AKBuf(server.core);
            buf->write(data,bytes);
            buf->setPos(0);
            sendList.push_back(buf);
            continueSend();
        }

        void AKServerSession::send(ak::container::AKBuf &buf)
        {
            if(buf.size()==0)return;
            ak::container::AKBuf *newBuf=new ak::container::AKBuf(server.core);
            newBuf->append(buf);
            newBuf->setPos(0);
            sendList.push_back(newBuf);
            continueSend();
        }

        void AKServerSession::close()
        {
            sock.close();
        }

        void AKServerSession::continueRecv()
        {
            if(recvSign)return;
            recvSign=true;
            sock.async_read_some(
                    boost::asio::buffer(recvBuf,recvBufBytes),
                    boost::bind(&AKServerSession::handle_read,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
            );
        }

        void AKServerSession::continueSend()
        {
            if(sendSign)return;
            if(sendList.size()>0)
            {
                uint32 sendBufSpareBytes=sendBufBytes;
                uint32 pos=0;
                sendSign=true;
                while(sendBufSpareBytes>0&&sendList.size()>0)
                {
                    ak::container::AKBuf *buf=*sendList.begin();
                    uint32 spareSendBytes=buf->size()-buf->getPos();
                    if(spareSendBytes<=sendBufSpareBytes)//能发完
                    {
                        buf->read(sendBuf+pos,spareSendBytes);
                        sendBufSpareBytes-=spareSendBytes;
                        pos+=spareSendBytes;
                        sendList.pop_front();
                        delete buf;
                    }
                    else//发不完
                    {
                        buf->read(sendBuf+pos,sendBufSpareBytes);
                        pos+=sendBufSpareBytes;
                        break;
                    }
                }
                boost::asio::async_write(
                        sock,
                        boost::asio::buffer(sendBuf,pos),
                        boost::bind(&AKServerSession::handle_write,this, boost::asio::placeholders::error)
                );
            }
            else
            {
                sendSign=false;
            }
        }

        void AKServerSession::handle_read(const boost::system::error_code& error,size_t bytes_transferred)
        {
            recvSign=false;
            if(!error)
            {
                server.onData(this,recvBuf,bytes_transferred);
                continueRecv();
            }
            else
            {
                deleteThis();
            }
        }

        void AKServerSession::handle_write(const boost::system::error_code& error)
        {
            sendSign=false;
            if(!error)
            {
                continueSend();
            }
            else
            {
                deleteThis();
            }
        }

        void AKServerSession::deleteThis()
        {
            if(sendSign==false&&recvSign==false)
            {
                delete this;
            }
        }
    }
}