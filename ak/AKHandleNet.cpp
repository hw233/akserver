#include "ak.h"
#include "AKHandleNet.h"
#include "AKHandleBuf.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleNet::AKHandleNet()
    {
    }

    AKHandleNet::~AKHandleNet()
    {
    }

    void AKHandleNet::init(AKScript *script)
    {
        AKHandler::init(script);

        //域名转ip
        AKQuickBindScriptFunc("domain2ip",&AKHandleNet::domain2ip,"ip=domain2ip(domain);//域名转ip");

        //client api
        AKQuickBindScriptFunc("cCreate",&AKHandleNet::cCreate,"client=cCreate(domain,port,recvBufSize);//创建client");
        AKQuickBindScriptFunc("cRemove",&AKHandleNet::cRemove,"cRemove(client);//删除client");
        AKQuickBindScriptFunc("cStart",&AKHandleNet::cStart,"isSucc=cStart(client);//启动client（不建议使用）");
        AKQuickBindScriptFunc("cClose",&AKHandleNet::cClose,"cClose(client);//关闭client");
        AKQuickBindScriptFunc("cIsClose",&AKHandleNet::cIsClose,"status=cIsClose(client);//查询client是否关闭或断线");
        AKQuickBindScriptFunc("cReqRecv",&AKHandleNet::cReqRecv,"cReqRecv(client);//client请求接收数据");
        AKQuickBindScriptFunc("cGetRecvSize",&AKHandleNet::cGetRecvSize,"recvSize=cGetRecvSize(client);//获取client接收数据尺寸");
        AKQuickBindScriptFunc("cRecv",&AKHandleNet::cRecv,"cRecv(client,buf,size);//client接收数据");
        AKQuickBindScriptFunc("cPop",&AKHandleNet::cPop,"cPop(client,popSize);//client弹出接收缓冲中的数据");
        AKQuickBindScriptFunc("cSend",&AKHandleNet::cSend,"cSend(client,dataBuf0[,dataBuf1,...dataBuf9]);//client发送");
        AKQuickBindScriptFunc("cIP",&AKHandleNet::cIP,"ip=cIP(client);//获取client的IP");
        AKQuickBindScriptFunc("cGetSendBufSize",&AKHandleNet::cGetSendBufSize,"bytes=cGetSendBufSize(client);//获取client的发送缓冲区剩余没有提交发送的字节数");
        AKQuickBindScriptFunc("cCnn",&AKHandleNet::cCnn,"cCnn(client);//连接（异步启动client）");
        AKQuickBindScriptFunc("cHasCnn",&AKHandleNet::cHasCnn,"isExist=cHasCnn(client,isSucc);//判断是否存在连接事件,isSucc：0=失败;1=成功;");
        AKQuickBindScriptFunc("cHasLost",&AKHandleNet::cHasLost,"isExist=cHasLost(client);//判断是否存在断线事件");
        AKQuickBindScriptFunc("cHasRecv",&AKHandleNet::cHasRecv,"isExist=cHasRecv(client);//判断是否存在接收事件");
        AKQuickBindScriptFunc("cSetDomain",&AKHandleNet::cSetDomain,"cSetDomain(client,domain);//重新设置连接域名");
        AKQuickBindScriptFunc("cSetPort",&AKHandleNet::cSetPort,"cSetPort(client,port);//重新设置连接端口");

        //server api
        AKQuickBindScriptFunc("sCreate",&AKHandleNet::sCreate,"server=sCreate(bindIP,port,clientMaxCount,keepAcceptCount,recvBufSize);//创建server");
        AKQuickBindScriptFunc("sStart",&AKHandleNet::sStart,"sStart(server);//启动server");
        AKQuickBindScriptFunc("sClose",&AKHandleNet::sClose,"sClose(server);//关闭server");
        AKQuickBindScriptFunc("sIsClose",&AKHandleNet::sIsClose,"status=sIsClose(server);//查询server是否关闭");
        AKQuickBindScriptFunc("sAccept",&AKHandleNet::sAccept,"clientID=sAccept(server);//获取server接受的连接,没有接受连接返回0");
        AKQuickBindScriptFunc("sReqRecv",&AKHandleNet::sReqRecv,"sReqRecv(server,clientID);//server请求接收数据");
        AKQuickBindScriptFunc("sHasRecv",&AKHandleNet::sHasRecv,"clientID=sHasRecv(server);//server是否接收到数据");
        AKQuickBindScriptFunc("sGetRecvSize",&AKHandleNet::sGetRecvSize,"recvSize=sGetRecvSize(server,clientID);//获取server接收数据尺寸");
        AKQuickBindScriptFunc("sRecv",&AKHandleNet::sRecv,"sRecv(server,clientID,buf,size);//server接收数据");
        AKQuickBindScriptFunc("sPop",&AKHandleNet::sPop,"sPop(server,clientID,popSize);//server弹出接收缓冲中的数据");
        AKQuickBindScriptFunc("sGetLost",&AKHandleNet::sGetLost,"clientID=sGetLost(server);//server获取断线");
        AKQuickBindScriptFunc("sCloseClient",&AKHandleNet::sCloseClient,"sCloseClient(server,clientID);//server关闭client");
        AKQuickBindScriptFunc("sSend",&AKHandleNet::sSend,"sSend(server,clientID,dataBuf0[,dataBuf1,...dataBuf9]);//server发送");
        AKQuickBindScriptFunc("sIP",&AKHandleNet::sIP,"sIP(server,clientID);//获取client的IP");
        AKQuickBindScriptFunc("sActiveClientIDArr",&AKHandleNet::sActiveClientIDArr,"clientIDArr=sActiveClientIDArr(server);//获取活动client的ID数组");

        //server2 api
        AKQuickBindScriptFunc("s2Create",&AKHandleNet::s2Create,"server=s2Create(bindIP,port);//s2创建tcp服务器");
        AKQuickBindScriptFunc("s2HasConnect",&AKHandleNet::s2HasConnect,"hasConnect=s2HasConnect(server,sessionIDArr);//s2获取接受连接的会话ID数组");
        AKQuickBindScriptFunc("s2HasDisconnect",&AKHandleNet::s2HasDisconnect,"hasDisconnect=s2HasDisconnect(server,sessionIDArr);//s2获取断开连接的会话ID数组");
        AKQuickBindScriptFunc("s2HasData",&AKHandleNet::s2HasData,"hasData=s2HasData(server,sessionIDArr);//s2获取收到数据的会话ID数组");
        AKQuickBindScriptFunc("s2GetData",&AKHandleNet::s2GetData,"s2GetData(server,sessionID,buf);//s2获取收到的数据");
        AKQuickBindScriptFunc("s2Send",&AKHandleNet::s2Send,"s2Send(server,sessionID,buf);//s2发送数据");
        AKQuickBindScriptFunc("s2CloseClient",&AKHandleNet::s2CloseClient,"s2CloseClient(server,sessionID);//s2关闭客户端");
        AKQuickBindScriptFunc("s2CtrlSend",&AKHandleNet::s2CtrlSend,"s2CtrlSend(server,sendMaxBytes);//s2控制发送数据字节数,如果发送缓冲中字节数超过sendMaxBytes则会断开连接");
        AKQuickBindScriptFunc("s2SessionIP",&AKHandleNet::s2SessionIP,"ip=s2SessionIP(server,sessionID);//s2获取客户端IP");

        //websocket api
        AKQuickBindScriptFunc("webSocketShake",&AKHandleNet::webSocketShake,"status=s2WebSocketShake(srcBuf,responseBuf);//websocket握手，解释srcBuf中的数据，检查是否握手成功，返回：成功返回解释结束的位置和responseBuf回复报文，-1=数据未完整;-2=失败;");
        AKQuickBindScriptFunc("webSocketDecode",&AKHandleNet::webSocketDecode,"status=webSocketDecode(srcBuf,offset,destBuf);//websocket解码，用于解释收到的数据，将srcBuf中的原数据取出并保存到destBuf，返回：成功返回解码结束的位置，-1=数据未完整;-2=失败;-3=请求关闭连接;<100=其它;");
        AKQuickBindScriptFunc("webSocketEncode",&AKHandleNet::webSocketEncode,"destBuf=webSocketEncode(srcBuf,destBuf);//websocket编码，用于打包准备发送的原数据");

        //client2 api
        AKQuickBindScriptFunc("c2Create",&AKHandleNet::c2Create,"client=c2Create();//c2创建tcp客户端");
        AKQuickBindScriptFunc("c2Destroy",&AKHandleNet::c2Destroy,"c2Destroy(client);//c2销毁tcp客户端");
        AKQuickBindScriptFunc("c2Connect",&AKHandleNet::c2Connect,"c2Connect(client,domain,port);//c2客户端连接服务端成功查询");
        AKQuickBindScriptFunc("c2HasConnect",&AKHandleNet::c2HasConnect,"hasConnect=c2HasConnect(client);//c2连接成功查询");
        AKQuickBindScriptFunc("c2HasNetError",&AKHandleNet::c2HasNetError,"c2HasNetError=c2HasDisconnect(client);//c2网络异常");
        AKQuickBindScriptFunc("c2HasData",&AKHandleNet::c2HasData,"hasData=c2HasData(client);//c2获取数据查询");
        AKQuickBindScriptFunc("c2GetData",&AKHandleNet::c2GetData,"c2GetData(client,buf);//c2获取收到的数据");
        AKQuickBindScriptFunc("c2Send",&AKHandleNet::c2Send,"c2Send(client,buf);//c2发送数据");
        AKQuickBindScriptFunc("c2Close",&AKHandleNet::c2Close,"c2Close(client);//c2关闭");
    }

    //s2获取客户端IP
    bool AKHandleNet::s2SessionIP(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        std::string ip;

        //会话ID
        uint32 sessionID=order->getParam(2)->toUint();
        ak::net2::AKServerForScript::SessionIDMap::iterator it=server->sessionIDMap.find(sessionID);
        if(it!=server->sessionIDMap.end())
        {
            ak::net2::AKServerSession *session=it->second;
            ip=session->ip();
        }

        out->setValue(ip.c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2发送数据
    bool AKHandleNet::c2Close(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        switch(script->getOrderStatus())
        {
            case 0:
            {
                net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
                if(!getScript()->getCore()->checkClient2(client))
                {
                    onRunError(order,"无效client");
                    return false;//暂停执行脚本
                }
                client->close();
                script->setOrderStatus(1);
                break;
            }
            default:
                out->setValue((double)1);
                script->nextOrder();//下一行指令
                return true;//继续执行脚本
        }
        return false;//暂停执行脚本
    }

    //c2发送数据
    bool AKHandleNet::c2Send(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        if(!getScript()->getCore()->checkClient2(client))
        {
            onRunError(order,"无效client");
            return false;//暂停执行脚本
        }

        //获取数据的缓冲
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }
        client->send(*buf);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2获取收到的数据
    bool AKHandleNet::c2GetData(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        if(!getScript()->getCore()->checkClient2(client))
        {
            onRunError(order,"无效client");
            return false;//暂停执行脚本
        }

        //获取数据的缓冲
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }
        buf->setEnd();
        bool succ=false;
        if(client->recvBuf.size()>0)
        {
            buf->append(client->recvBuf);
            client->recvBuf.clear();
            succ=true;
        }
        if(succ)out->setValue((double)1);
        else out->setValue((double)0);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2获取数据查询
    bool AKHandleNet::c2HasData(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        if(!getScript()->getCore()->checkClient2(client))
        {
            onRunError(order,"无效client");
            return false;//暂停执行脚本
        }

        //连接
        if(client->recvSign)
        {
            client->recvSign=false;
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2网络异常
    bool AKHandleNet::c2HasNetError(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        if(!getScript()->getCore()->checkClient2(client))
        {
            onRunError(order,"无效client");
            return false;//暂停执行脚本
        }

        //连接
        if(client->netErrorSign)
        {
            client->netErrorSign=false;
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2连接成功
    bool AKHandleNet::c2HasConnect(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        if(!getScript()->getCore()->checkClient2(client))
        {
            onRunError(order,"无效client");
            return false;//暂停执行脚本
        }

        //连接
        if(client->connectSign)
        {
            client->connectSign=false;
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2客户端连接服务端
    bool AKHandleNet::c2Connect(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        if(!getScript()->getCore()->checkClient2(client))
        {
            onRunError(order,"无效client");
            return false;//暂停执行脚本
        }

        //域名
        const int8 *domain=order->getParam(2)->toString();

        //端口
        uint16 port=order->getParam(3)->toUint();

        //连接
        client->connect(net2::AKClient2::domain2IP(domain).c_str(),port);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2销毁tcp客户端
    bool AKHandleNet::c2Destroy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKClientForScript *client=(net2::AKClientForScript *)order->getParam(1)->toHandle();
        getScript()->getCore()->destroyClient2(client);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //c2创建tcp客户端
    bool AKHandleNet::c2Create(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        net2::AKClientForScript *client=getScript()->getCore()->createClient2();

        out->setValue((void *)client);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //websocket握手，解释srcBuf中的数据，检查是否握手成功，返回：成功返回解释结束的位置和responseBuf回复报文，-1=数据未完整;-2=失败;
    bool AKHandleNet::webSocketShake(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double status=0;

        //原数据缓冲
        container::AKBuf *srcBuf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(srcBuf))
        {
            onRunError(order,"无效srcBuf");
            return false;//暂停执行脚本
        }
        uint32 bytes=srcBuf->size();
        if(bytes==0)
        {
            status=-1;//未完整
            //goto webSocketShake_end;
        }
        else
        {
            //回复报文缓冲
            container::AKBuf *responseBuf=(container::AKBuf *)order->getParam(2)->toHandle();
            if(!script->getCore()->checkBuf(responseBuf))
            {
                onRunError(order,"无效responseBuf");
                return false;//暂停执行脚本
            }
            responseBuf->clear();

            //解释
            container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
            int8 *tmp=mem.alloc(bytes);
            srcBuf->setPos(0);
            srcBuf->read(tmp,bytes);
            std::string str(tmp,bytes);
            int32 pos=str.find("\r\n\r\n");
            int32 msgBytes=pos+4;//报文长度
            if(pos<0)
            {
                status=-1;//未完整
                //goto webSocketShake_end;
            }
            else
            {
                const int8 *keyWord="Sec-WebSocket-Key";
                pos=str.find(keyWord);
                if(pos<0)
                {
                    status=-2;//失败
                    //goto webSocketShake_end;
                }
                else
                {
                    pos=str.find(":",pos+17);
                    if(pos<0)
                    {
                        status=-2;//失败
                        //goto webSocketShake_end;
                    }
                    else
                    {
                        pos+=1;
                        int32 pos2=str.find("\r\n",pos);
                        if(pos2<0)
                        {
                            status=-2;//失败
                            //goto webSocketShake_end;
                        }
                        else
                        {
                            std::string key=str.substr(pos,pos2-pos);
                            AKCommon::strReplace(key," ","");
                            const int8 *magic="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
                            key+=magic;
                            std::string ttt=AKCommon::sha1(key.c_str(),key.size());
                            AKCommon::hex2buf(responseBuf,ttt);
                            bytes=responseBuf->size();
                            tmp=mem.alloc(bytes);
                            responseBuf->read(tmp,bytes);
                            std::string tt(tmp,bytes);
                            std::string rtKey=AKCommon::base64(tt);
                            std::string responseMsg="HTTP/1.1 101 WebSocket Protocol Handshake\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
                            responseMsg+=rtKey;
                            responseMsg+="\r\nSec-WebSocket-Version: 13\r\n\r\n";
                            responseBuf->clear();
                            responseBuf->write(responseMsg.c_str(),responseMsg.size());
                            status=msgBytes;
                        }
                    }
                }
            }
        }

        out->setValue(status);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //websocket解码，用于解释收到的数据，将srcBuf中的原数据取出并保存到destBuf，返回：成功返回解码结束的位置，-1=数据未完整;-2=失败;-3=请求关闭连接;<100=其它;
    bool AKHandleNet::webSocketDecode(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        //原数据缓冲
        container::AKBuf *srcBuf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(srcBuf))
        {
            onRunError(order,"无效srcBuf");
            return false;//暂停执行脚本
        }

        double status=-1;

        //数据偏移
        uint32 offset=order->getParam(2)->toUint();
        if(offset<srcBuf->size())
        {
            //目标数据缓冲
            container::AKBuf *destBuf=(container::AKBuf *)order->getParam(3)->toHandle();
            if(!script->getCore()->checkBuf(destBuf))
            {
                onRunError(order,"无效destBuf");
                return false;//暂停执行脚本
            }
            destBuf->clear();

            uint32 tt;
            uint32 spare;

            //解释
            srcBuf->setPos(offset);
            //解释第1个字节
            uint8 t;
            if(srcBuf->readUint8(t))
            {
                uint8 isEof=t>>7;//msg是否结束
                uint8 type=t&(0xf);//msg类型
                //解释第2个字节
                if(srcBuf->readUint8(t))
                {
                    uint8 hasMask=t>>7;//msg是否存在掩码
                    uint32 msgSize=t&0x7f;//msg尺寸
                    //扩展尺寸
                    if(msgSize==126)//msg尺寸由后2个字节表示（16位）
                    {
                        uint32 high;
                        if(srcBuf->readUint8(t))
                        {
                            high=t;
                            uint32 low;
                            if(srcBuf->readUint8(t))
                            {
                                low=t;
                                msgSize=low+(high<<8);
                            }
                        }
                    }
                    else if(msgSize==127)//msg尺寸由后8个字节表示（64位）
                    {
                        spare=srcBuf->size()-srcBuf->getPos();
                        if(spare>=8)
                        {
                            srcBuf->readUint32(tt);//高32位//big endian
                            //低32位//big endian
                            uint8 tt0,tt1,tt2,tt3;
                            srcBuf->readUint8(tt0);
                            srcBuf->readUint8(tt1);
                            srcBuf->readUint8(tt2);
                            srcBuf->readUint8(tt3);
                            uint32 t0=tt0;
                            uint32 t1=tt1;
                            uint32 t2=tt2;
                            uint32 t3=tt3;
                            msgSize=(t0<<24)+(t1<<16)+(t2<<8)+t3;
                        }
                    }
                    //计算是否收到完整的消息
                    spare=srcBuf->size()-srcBuf->getPos();
                    uint32 needSize=msgSize;
                    if(hasMask)needSize+=4;
                    if(spare>=needSize)
                    {
                        //解出消息内容
                        if(hasMask)//存在掩码
                        {
                            uint8 mask[4];
                            srcBuf->readUint8(mask[0]);
                            srcBuf->readUint8(mask[1]);
                            srcBuf->readUint8(mask[2]);
                            srcBuf->readUint8(mask[3]);
                            //解码
                            for(uint32 i=0;i<msgSize;++i)
                            {
                                srcBuf->readUint8(t);
                                t=t^mask[i%4];
                                destBuf->writeUint8(t);
                            }
                        }
                        else//无掩码
                        {
                            destBuf->append(*srcBuf,srcBuf->getPos(),msgSize);
                        }
                        if(type==1||type==2)//1=文本msg;2=二进制msg;
                        {
                            status=srcBuf->getPos();
                        }
                        else if(type==8)//请求关闭连接
                        {
                            status=-3;
                        }
                        else//其它//9=ping;10=pong;
                        {
                            int32 pos=srcBuf->getPos();
                            status=(-pos)-100;
                        }
                    }
                }
            }
        }

        out->setValue(status);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //websocket编码，用于打包准备发送的原数据
    bool AKHandleNet::webSocketEncode(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        //原数据缓冲
        container::AKBuf *srcBuf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(srcBuf))
        {
            onRunError(order,"无效srcBuf");
            return false;//暂停执行脚本
        }

        //目标数据缓冲
        container::AKBuf *destBuf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(destBuf))
        {
            onRunError(order,"无效destBuf");
            return false;//暂停执行脚本
        }
        destBuf->clear();

        uint32 msgSize=srcBuf->size();
        if(msgSize>0)
        {
            //二进制msg
            destBuf->writeUint8(0x82);
            if(msgSize<=125)//不需要扩展msg长度
            {
                //trace("wsSend bytes <=125");
                //不加掩码
                destBuf->writeUint8(msgSize);
            }
            else if(msgSize<=65535)//16位msg长度
            {
                //trace("wsSend bytes <=65535");
                //不加掩码
                destBuf->writeUint8(126);
                //写入扩展长度//big endian
                destBuf->writeUint8(msgSize>>8);
                destBuf->writeUint8(msgSize&0xff);
            }
            else//64位msg长度
            {
                //trace("wsSend bytes >65535");
                //不加掩码
                destBuf->writeUint8(127);
                //写入扩展长度
                //高32位//big endian
                destBuf->writeUint32(0);
                //低32位//big endian
                destBuf->writeUint8(msgSize>>24);
                destBuf->writeUint8(msgSize>>16&0xff);
                destBuf->writeUint8(msgSize>>8&0xff);
                destBuf->writeUint8(msgSize&0xff);
            }
            destBuf->append(*srcBuf);
        }

        out->setValue((void *)destBuf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建tcp服务器
    bool AKHandleNet::s2Create(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        std::string &domain=order->getParam(1)->toStdString();
        uint16 port=order->getParam(2)->toUint();//端口

        net2::AKServerForScript *server=getScript()->getCore()->createServer2(domain.c_str(),port);

        out->setValue((void *)server);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //tcp服务器是否有新连接，并返回sessionID数组
    bool AKHandleNet::s2HasConnect(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        container::AKArr *arr=(container::AKArr *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }
        arr->clear();

        net2::AKServerForScript::Uint32Uint32Map &map=server->connectSessionIDMap;
        if(map.size()>0)
        {
            net2::AKServerForScript::Uint32Uint32Map::iterator it=map.begin();
            while(it!=map.end())
            {
                uint32 sessionID=it->first;
                arr->push(sessionID);
                ++it;
            }
            map.clear();
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //tcp服务器是否有断开的连接，并返回sessionID数组
    bool AKHandleNet::s2HasDisconnect(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        container::AKArr *arr=(container::AKArr *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }
        arr->clear();

        net2::AKServerForScript::Uint32Uint32Map &map=server->disconnectSessionIDMap;
        if(map.size()>0)
        {
            net2::AKServerForScript::Uint32Uint32Map::iterator it=map.begin();
            while(it!=map.end())
            {
                uint32 sessionID=it->first;
                arr->push(sessionID);
                ++it;
            }
            map.clear();
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //tcp服务器是否有收到数据的连接，并返回sessionID数组
    bool AKHandleNet::s2HasData(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        container::AKArr *arr=(container::AKArr *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }
        arr->clear();

        net2::AKServerForScript::Uint32Uint32Map &map=server->recvSessionIDMap;
        if(map.size()>0)
        {
            net2::AKServerForScript::Uint32Uint32Map::iterator it=map.begin();
            while(it!=map.end())
            {
                uint32 sessionID=it->first;
                arr->push(sessionID);
                ++it;
            }
            map.clear();
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //tcp服务器获取收到的数据
    bool AKHandleNet::s2GetData(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        //会话ID
        uint32 sessionID=order->getParam(2)->toUint();

        //获取数据的缓冲
        container::AKBuf *buf=(container::AKBuf *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }
        buf->setEnd();
        bool succ=false;
        ak::net2::AKServerForScript::SessionIDMap::iterator it=server->sessionIDMap.find(sessionID);
        if(it!=server->sessionIDMap.end())
        {
            ak::net2::AKServerSession *session=it->second;
            ak::net2::AKServerForScript::SessionInfo *info=server->sessionMap[session];
            if(info->recvBuf.size()>0)
            {
                buf->append(info->recvBuf);
                info->recvBuf.clear();
                succ=true;
            }

        }
        if(succ)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //tcp服务器发送数据
    bool AKHandleNet::s2Send(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        //会话ID
        uint32 sessionID=order->getParam(2)->toUint();

        //获取数据的缓冲
        container::AKBuf *buf=(container::AKBuf *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }
        ak::net2::AKServerForScript::SessionIDMap::iterator it=server->sessionIDMap.find(sessionID);
        if(it!=server->sessionIDMap.end())
        {
            ak::net2::AKServerSession *session=it->second;
            session->send(*buf);
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //tcp服务器关闭客户端
    bool AKHandleNet::s2CloseClient(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        switch(script->getOrderStatus())
        {
            case 0:
            {
                net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
                if(!script->getCore()->checkServer2(server))
                {
                    onRunError(order,"无效server句柄");
                    return false;//暂停执行脚本
                }

                //会话ID
                uint32 sessionID=order->getParam(2)->toUint();

                ak::net2::AKServerForScript::SessionIDMap::iterator it=server->sessionIDMap.find(sessionID);
                if(it!=server->sessionIDMap.end())
                {
                    ak::net2::AKServerSession *session=it->second;
                    session->close();
                }
                script->setOrderStatus(1);
                break;
            }
            default:
                out->setValue((double)1);
                script->nextOrder();//下一行指令
                return true;//继续执行脚本
        }
        return false;//暂停执行脚本
    }

    //s2控制发送数据字节数,如果发送缓冲中字节数超过sendMaxBytes则会断开连接
    bool AKHandleNet::s2CtrlSend(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net2::AKServerForScript *server=(net2::AKServerForScript *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer2(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        //最大发送字节数
        uint32 sendMaxBytes=order->getParam(2)->toUint();

        //监控
        ak::net2::AKServer2::SessionList *list=server->getSessionList();
        ak::net2::AKServer2::SessionList::iterator it=list->begin();
        while(it!=list->end())
        {
            uint32 totalBytes=0;
            ak::net2::AKServerSession *session=*it;
            ak::net2::AKServerSession::SendList::iterator it2=session->sendList.begin();
            while(it2!=session->sendList.end())
            {
                ak::container::AKBuf *buf=*it2;
                totalBytes+=buf->size();
                ++it2;
            }
            if(totalBytes>=sendMaxBytes)
            {
                session->close();
            }
            ++it;
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //重新设置连接端口
    bool AKHandleNet::cSetPort(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        //端口
        uint32 port=order->getParam(2)->toUint();

        client->mPort=port;

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //重新设置连接域名
    bool AKHandleNet::cSetDomain(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        //域名
        const int8 *domain=order->getParam(2)->toString();

        client->mDomain=domain;

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断是否存在接收事件
    bool AKHandleNet::cHasRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        if(client->hasRecv())
        {
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //连接（异步启动client）
    bool AKHandleNet::cCnn(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        client->start();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断是否存在断线事件
    bool AKHandleNet::cHasLost(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        if(client->hasLost())out->setValue((double)1);
        else out->setValue((double)0);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断是否存在连接事件,isSucc：0=失败;1=成功;
    bool AKHandleNet::cHasCnn(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }
        AKVar *isSuccVar=order->getParam(2);

        bool isSucc=false;
        if(client->hasCnn(isSucc))out->setValue((double)1);
        else out->setValue((double)0);

        if(isSucc)isSuccVar->setValue((double)1);
        else isSuccVar->setValue((double)0);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //域名转ip
    bool AKHandleNet::domain2ip(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        const int8 *domain=order->getParam(1)->toString();
        std::string ip=AKCommon::domain2IP(domain);

        out->setValue(ip);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取活动client的ID数组
    bool AKHandleNet::sActiveClientIDArr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        AKUint32Array idArr;
        server->getActiveClientIDArr(idArr);

        container::AKArr *arr=script->getCore()->createArr();
        uint32 c=idArr.size();
        for(uint32 i=0;i<c;++i)
        {
            arr->push((double)idArr[i]);
        }

        out->setValue((void *)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取client的IP
    bool AKHandleNet::sIP(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        std::string ip=server->getClientIP(clientID);

        out->setValue(ip);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server发送
    bool AKHandleNet::sSend(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        uint32 bufCount=order->varParams.size()-3;
        if(bufCount>10)
        {
            onRunError(order,"最多连续发送10个数据buf");
            return false;//暂停执行脚本
        }

        container::AKBuf *bufArray[10];
        container::AKBuf *buf=NULL;
        for(uint32 i=0;i<bufCount;++i)
        {
            buf=(container::AKBuf *)order->getParam(i+3)->toHandle();
            if(!script->getCore()->checkBuf(buf))
            {
                onRunError(order,"无效buf");
                return false;//暂停执行脚本
            }
            bufArray[i]=buf;
        }

        server->send(clientID,bufArray,bufCount);
        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server关闭client
    bool AKHandleNet::sCloseClient(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        server->closeClient(clientID);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server获取断线
    bool AKHandleNet::sGetLost(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        net::AKClientProxy *proxy=server->getLost();
        if(proxy)
        {
            out->setValue((double)proxy->getID());
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server弹出接收缓冲中的数据
    bool AKHandleNet::sPop(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        uint32 popSize=order->getParam(3)->toUint();//弹出字节数
        if(!server->pop(clientID,popSize))//弹出失败
        {
            onRunError(order,"server弹出接收数据失败");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server接收数据
    bool AKHandleNet::sRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        container::AKBuf *buf=(container::AKBuf *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 size=order->getParam(4)->toUint();
        if(size>server->getRecvSize(clientID))
        {
            onRunError(order,"server获取接收数据不足");
            return false;//暂停执行脚本
        }

        const int8 *recvData=server->getRecvData(clientID);
        buf->clear();
        if(recvData)
        {
            buf->write(recvData,size);
            buf->setPos(0);
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server是否接收到数据
    bool AKHandleNet::sHasRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        out->setValue((double)server->hasRecv());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取server接收数据尺寸
    bool AKHandleNet::sGetRecvSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        out->setValue((double)server->getRecvSize(clientID));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server请求接收数据
    bool AKHandleNet::sReqRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        uint32 clientID=order->getParam(2)->toUint();

        server->reqRecv(clientID);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //server接受连接
    bool AKHandleNet::sAccept(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        net::AKClientProxy *proxy=server->getAcceptedClientProxy();
        if(proxy)
        {
            out->setValue((double)proxy->getID());
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //查询server是否关闭
    bool AKHandleNet::sIsClose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        if(server->getStatus()==0)
        {
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //关闭server
    bool AKHandleNet::sClose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        switch(script->getOrderStatus())
        {
            case 0:
            {
                server->close();
                script->setOrderStatus(1);
                break;
            }
            default:
            {
                if(server->getStatus()==0)
                {
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
        }
        return false;//暂停执行脚本
    }

    //启动server
    bool AKHandleNet::sStart(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKServer *server=(net::AKServer *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkServer(server))
        {
            onRunError(order,"无效server句柄");
            return false;//暂停执行脚本
        }

        switch(script->getOrderStatus())
        {
            case 0:
            {
                if(server->getStatus()==0)//未启动
                {
                    server->mIsStartComplete=false;
                    server->start();
                    script->setOrderStatus(1);
                }
                else
                {
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
            default:
            {
                if(server->mIsStartComplete)
                {
                    if(server->getStatus()==0)
                    {
                        out->setValue((double)0);
                    }
                    else
                    {
                        out->setValue((double)1);
                    }
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
        }
        return false;//暂停执行脚本
    }

    //创建server
    bool AKHandleNet::sCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(5,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *bindIP=order->getParam(1)->toString();//绑定ip
        uint32 port=order->getParam(2)->toUint();//端口
        uint32 clientMaxCount=order->getParam(3)->toUint();//最大client数量
        uint32 keepAcceptCount=order->getParam(4)->toUint();//保持accept数量
        uint32 recvBufsize=order->getParam(5)->toUint();//接收缓冲尺寸

        net::AKServer *server=script->getCore()->createServer(script->getProcess(),bindIP,port,clientMaxCount,keepAcceptCount,recvBufsize);

        out->setValue((void *)server);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //client接收数据
    bool AKHandleNet::cRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 size=order->getParam(3)->toUint();
        if(size>client->getRecvSize())
        {
            onRunError(order,"client获取接收数据不足");
            return false;//暂停执行脚本
        }

        buf->clear();
        buf->write(client->getRecvData(),size);
        buf->setPos(0);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //client弹出接收缓冲中的数据
    bool AKHandleNet::cPop(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        uint32 popSize=order->getParam(2)->toUint();//弹出字节数
        if(!client->pop(popSize))//弹出失败
        {
            onRunError(order,"client弹出接收数据失败");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取client的IP
    bool AKHandleNet::cIP(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        out->setValue(client->ip());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取client的发送缓冲区剩余没有提交发送的字节数
    bool AKHandleNet::cGetSendBufSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        out->setValue((double)client->mSendBuf.size());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //client发送
    bool AKHandleNet::cSend(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        uint32 bufCount=order->varParams.size()-2;
        if(bufCount>10)
        {
            onRunError(order,"最多连续发送10个数据buf");
            return false;//暂停执行脚本
        }

        container::AKBuf *bufArray[10];
        container::AKBuf *buf=NULL;
        for(uint32 i=0;i<bufCount;++i)
        {
            buf=(container::AKBuf *)order->getParam(i+2)->toHandle();
            if(!script->getCore()->checkBuf(buf))
            {
                onRunError(order,"无效buf");
                return false;//暂停执行脚本
            }
            bufArray[i]=buf;
        }

        if(client->getStatus()==2)//获取状态:0=未启动;1=正在启动;2=启动成功;
        {
            client->send(bufArray,bufCount);
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取client接收数据尺寸
    bool AKHandleNet::cGetRecvSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        out->setValue((double)client->getRecvSize());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //client请求接收数据
    bool AKHandleNet::cReqRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        client->reqRecv();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //关闭client
    bool AKHandleNet::cClose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        switch(script->getOrderStatus())
        {
            case 0:
            {
                client->close();
                script->setOrderStatus(1);
                break;
            }
            default:
            {
                if(client->getStatus()==0)
                {
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
        }
        return false;//暂停执行脚本
    }

    //启动client
    bool AKHandleNet::cStart(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        switch(script->getOrderStatus())
        {
            case 0:
            {
                if(client->getStatus()==0)//未启动
                {
                    client->mIsConnectComplete=false;
                    client->start();
                    script->setOrderStatus(1);
                }
                else
                {
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
            default:
            {
                if(client->mIsConnectComplete)
                {
                    if(client->getStatus()==0)
                    {
                        out->setValue((double)0);
                    }
                    else
                    {
                        out->setValue((double)1);
                    }
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
        }
        return false;//暂停执行脚本
    }

    //查询client是否关闭或断线
    bool AKHandleNet::cIsClose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        if(client->getStatus()==0)
        {
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建client
    bool AKHandleNet::cCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *domain=order->getParam(1)->toString();
        uint32 port=order->getParam(2)->toUint();
        uint32 recvBufsize=order->getParam(3)->toUint();

        net::AKClient *client=script->getCore()->createClient(script->getProcess(),domain,port,recvBufsize);

        out->setValue((void *)client);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除client
    bool AKHandleNet::cRemove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        net::AKClient *client=(net::AKClient *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkClient(client))
        {
            onRunError(order,"无效client句柄");
            return false;//暂停执行脚本
        }

        switch(script->getOrderStatus())
        {
            case 0:
            {
                client->close();
                script->setOrderStatus(1);
                break;
            }
            default:
            {
                if(client->getStatus()==0)
                {
                    script->getCore()->removeClient(client);
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
            }
        }
        return false;//暂停执行脚本
    }

}
