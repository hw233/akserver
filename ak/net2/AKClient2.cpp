#include "ak.h"
#include "AKClient2.h"

namespace ak
{
    namespace net2
    {
        AKClient2::AKClient2(boost::asio::io_service &io,ak::AKCore *core):
                io(io),
                sock(io),
                connectSign(0),
                sendSign(false),
                recvSign(false),
                core(core)
        {
        }

        AKClient2::~AKClient2()
        {
            SendList::iterator it=sendList.begin();
            while(it!=sendList.end())
            {
                ak::container::AKBuf *buf=*it;
                delete buf;
                ++it;
            }
        }

        void AKClient2::connect(const int8 *ip,uint16 port)
        {
            if(connectSign!=0)return;//不是未连接状态
            connectSign=1;//正在连接
            try
            {
                boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(ip),port);
                sock.async_connect(ep,boost::bind(&AKClient2::handle_connect,this,boost::asio::placeholders::error));
            }
            catch(...)
            {
                handle_connect2(false);
            }
        }

        void AKClient2::send(const int8 *data,uint32 bytes)
        {
            if(bytes==0)return;
            ak::container::AKBuf *buf=new ak::container::AKBuf(core);
            buf->write(data,bytes);
            buf->setPos(0);
            sendList.push_back(buf);
            continueSend();
        }

        void AKClient2::send(ak::container::AKBuf &buf)
        {
            if(buf.size()==0)return;
            ak::container::AKBuf *newBuf=new ak::container::AKBuf(core);
            newBuf->append(buf);
            newBuf->setPos(0);
            sendList.push_back(newBuf);
            continueSend();
        }

        void AKClient2::close()
        {
            sock.close();
        }

        std::string AKClient2::domain2IP(const int8 *domain)
        {
            try
            {
                boost::asio::io_service io_service;
                boost::asio::ip::tcp::resolver resolver(io_service);
                boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),std::string(domain),"");

                boost::asio::ip::tcp::resolver::iterator endpoint_iterator=resolver.resolve(query);

                boost::asio::ip::tcp::endpoint end=endpoint_iterator->endpoint();
                boost::asio::ip::address addr=end.address();
                std::string ip=addr.to_string();
                return ip;
            }
            catch(...)
            {
                return std::string("");
            }
        }

        void AKClient2::handle_connect(const boost::system::error_code& error)
        {
            if(error)
                handle_connect2(false);
            else
                handle_connect2(true);
        }

        void AKClient2::handle_connect2(bool succSign)
        {
            if(!succSign)
            {
                close();
                connectSign=0;//未连接
                onConnect(false);
            }
            else
            {
                connectSign=2;//连接正常
                sendList.clear();
                onConnect(true);
                continueRecv();
            }
        }

        void AKClient2::continueRecv()
        {
            if(recvSign)return;
            recvSign=true;
            sock.async_read_some(
                    boost::asio::buffer(recvBuf,recvBufBytes),
                    boost::bind(&AKClient2::handle_read,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
            );
        }

        void AKClient2::continueSend()
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
                        boost::bind(&AKClient2::handle_write,this, boost::asio::placeholders::error)
                );
            }
            else
            {
                sendSign=false;
            }
        }

        void AKClient2::handle_read(const boost::system::error_code& error,size_t bytes_transferred)
        {
            recvSign=false;
            if(!error)
            {
                onData(recvBuf,bytes_transferred);
                continueRecv();
            }
            else
            {
                tryClose();
            }
        }

        void AKClient2::handle_write(const boost::system::error_code& error)
        {
            sendSign=false;
            if(!error)
            {
                continueSend();
            }
            else
            {
                tryClose();
            }
        }

        void AKClient2::tryClose()
        {
            close();
            if(recvSign==false&&sendSign==false)
            {
                connectSign=0;//未连接
                onDisconnect();
            }
        }
    }
}