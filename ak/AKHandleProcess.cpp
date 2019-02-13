#include "ak.h"
#include "AKScript.h"
#include "AKHandleBuf.h"
#include "AKHandleProcess.h"
#include "AKGlobal.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleProcess::AKHandleProcess()
            :mProcess(NULL),
             mScriptFunc(NULL),
             mHandleBuf(NULL),
             mParent(NULL)
    {
    }

    AKHandleProcess::~AKHandleProcess()
    {
    }

    AKHandleBuf *AKHandleProcess::getHandleBuf()
    {
        if(!mHandleBuf)
        {
            mHandleBuf=(AKHandleBuf *)getScript()->getHandler("AKHandleBuf");
        }
        return mHandleBuf;
    }

    void AKHandleProcess::reset()
    {

    }

    void AKHandleProcess::init(AKScript *script)
    {
        AKHandler::init(script);

        AKQuickBindScriptFunc("pSet",&AKHandleProcess::pSet,"pSet(key,val);//设置进程变量值");
        AKQuickBindScriptFunc("pGet",&AKHandleProcess::pGet,"val=pGet(key);//获取进程变量值");
        AKQuickBindScriptFunc("pGetRef",&AKHandleProcess::pGetRef,"ref=pGetRef(key);//获取进程变量引用");
        AKQuickBindScriptFunc("pRemove",&AKHandleProcess::pRemove,"pRemove(key);//删除进程变量");
        AKQuickBindScriptFunc("pExist",&AKHandleProcess::pExist,"isExist=pExist(key);//进程变量是否存在");

        AKQuickBindScriptFunc("coreID",&AKHandleProcess::coreID,"coreID=coreID();//获取核id");
        AKQuickBindScriptFunc("coreCount",&AKHandleProcess::coreCount,"count=coreCount();//获取核数量");
        AKQuickBindScriptFunc("sysCoreID",&AKHandleProcess::sysCoreID,"sysCoreID=sysCoreID();//获取系统预留核ID（用于系统任务）");

        AKQuickBindScriptFunc("handle",&AKHandleProcess::_handle,"pID=handle();//创建空句柄");
        AKQuickBindScriptFunc("empty",&AKHandleProcess::empty,"isEmpty=empty(handle);//判断句柄是否为空");
        AKQuickBindScriptFunc("pCreate",&AKHandleProcess::pCreate,"pID=pCreate(scriptUrl[,isBind,coreID,buf]);//创建进程//coreID代表指定核id");
        AKQuickBindScriptFunc("pCreateNoBreakPoint",&AKHandleProcess::pCreateNoBreakPoint,"pID=pCreateNoBreakPoint(scriptUrl[,isBind,coreID,buf]);//创建进程（禁止进程内断点）//coreID代表指定核id");
        AKQuickBindScriptFunc("pID",&AKHandleProcess::pID,"pID=pID();//获取进程id");
        AKQuickBindScriptFunc("pKill",&AKHandleProcess::pKill,"pKill(pID);//杀死进程");
        AKQuickBindScriptFunc("pBind",&AKHandleProcess::pBind,"pBind(pID);//绑定进程，被绑定的进程在当前进程死亡后也跟着死亡");

        AKQuickBindScriptFunc("pSend",&AKHandleProcess::pSend,"pSend(pID,msgDataBuf1[,msgDataBuf2,...]);//向进程pID发送msg");
        AKQuickBindScriptFunc("pRecv",&AKHandleProcess::pRecv,"rt=pRecv(pID,msgBuf);//接收进程pID发送的msg");

        AKQuickBindScriptFunc("pRequest",&AKHandleProcess::pRequest,"rt=pRequest(pID,timeout,rtDataBuf,msgDataBuf1[,msgDataBuf2,...]);//向进程pID请求数据，直到数据返回或超时为止");
        AKQuickBindScriptFunc("pHasRequest",&AKHandleProcess::pHasRequest,"rt=pHasRequest(pID,msgBuf);//检查是否收到进程pID的msg请求数据");
        AKQuickBindScriptFunc("pResponse",&AKHandleProcess::pResponse,"pResponse(pID,msgDataBuf1[,msgDataBuf2,...]);//向进程pID回复数据");

        AKQuickBindScriptFunc("func",&AKHandleProcess::func,"func1=func(scriptUrl);//创建脚本函数句柄");
        AKQuickBindScriptFunc("exec@2",&AKHandleProcess::exec2,"//为了支持直接使用func创建的函数句柄变量名或字符串变量名调用//不能在脚本中显式调用exec@2");
        AKQuickBindScriptFunc("input",&AKHandleProcess::input,"var=input(i);//获取第i个参数");
        AKQuickBindScriptFunc("inputCount",&AKHandleProcess::inputCount,"count=inputCount();//获取参数数量");
        AKQuickBindScriptFunc("return",&AKHandleProcess::_return,"return(rtVar);//返回变量给调用者");

        AKQuickBindScriptFunc("pDt",&AKHandleProcess::pDt,"dt=pDt();//当前进程中，返回与上次dt调用的间隔时间（毫秒）");

        //物理进程间通信API
        AKQuickBindScriptFunc("mqCreate",&AKHandleProcess::mqCreate,"isSucc=mqCreate(messageQueueName,msgMaxCount,msgMaxSize);//创建物理进程间消息队列");
        AKQuickBindScriptFunc("mqRemove",&AKHandleProcess::mqRemove,"isSucc=mqRemove(messageQueueName);//删除物理进程间消息队列");
        AKQuickBindScriptFunc("mqSend",&AKHandleProcess::mqSend,"isSucc=mqSend(messageQueueName,msgBuf);//发送到物理进程间消息队列");
        AKQuickBindScriptFunc("mqRecv",&AKHandleProcess::mqRecv,"isSucc=mqRecv(messageQueueName,msgBuf);//物理进程间消息队列接收");
    }

    //物理进程间消息队列接收
    bool AKHandleProcess::mqRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *messageQueueName=order->getParam(1)->toString();//消息队列名称
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        bool rt=true;

        try
        {
            boost::interprocess::message_queue mq(
                    boost::interprocess::open_only,
                    messageQueueName
            );

            container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
            int8 *tmp=mem.alloc(AKStaticMemSize);

            boost::interprocess::message_queue::size_type bytes;
            unsigned int priority;
            rt=mq.try_receive(tmp,AKStaticMemSize,bytes,priority);
            if(rt)
            {
                buf->clear();
                buf->write(tmp,bytes);
            }
        }
        catch(...)
        {
            rt=false;
        }

        if(rt)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //发送到物理进程间消息队列
    bool AKHandleProcess::mqSend(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *messageQueueName=order->getParam(1)->toString();//消息队列名称
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        bool rt=true;

        try
        {
            boost::interprocess::message_queue mq(
                    boost::interprocess::open_only,
                    messageQueueName
            );

            container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
            uint32 bytes=buf->size();
            int8 *tmp=mem.alloc(bytes);
            uint32 oldPos=buf->getPos();
            buf->setPos(0);
            buf->read(tmp,bytes);
            buf->setPos(oldPos);

            rt=mq.try_send(tmp,bytes,0);
        }
        catch(...)
        {
            rt=false;
        }

        if(rt)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除物理进程间消息队列
    bool AKHandleProcess::mqRemove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *messageQueueName=order->getParam(1)->toString();//消息队列名称

        bool rt=true;

        try
        {
            boost::interprocess::message_queue::remove(messageQueueName);
        }
        catch(...)
        {
            rt=false;
        }

        if(rt)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建物理进程间消息队列
    bool AKHandleProcess::mqCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *messageQueueName=order->getParam(1)->toString();//消息队列名称
        uint32 msgMaxCount=order->getParam(2)->toUint();//消息最大数量
        uint32 msgMaxSize=order->getParam(3)->toUint();//消息最大尺寸

        bool rt=true;

        try
        {
            boost::interprocess::message_queue mq(
                    boost::interprocess::open_or_create,
                    messageQueueName,
                    msgMaxCount,
                    msgMaxSize
            );
        }
        catch(...)
        {
            rt=false;
        }

        if(rt)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取参数数量
    bool AKHandleProcess::inputCount(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *posBuf=&script->mDataPosBuf;
        uint32 count=posBuf->size()/(sizeof(uint32)*2);


        out->setValue((double)count);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取第i个参数
    bool AKHandleProcess::input(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();

        uint32 i=order->getParam(1)->toUint();//第i个参数
        uint32 index=i*sizeof(uint32)*2;

        container::AKBuf *posBuf=&script->mDataPosBuf;
        if(index>=posBuf->size())
        {
            onRunError(order,"无效参数下标");
            return false;//暂停执行脚本
        }
        posBuf->setPos(index);
        uint32 pos,type;
        posBuf->readUint32(pos);//参数变量位置
        posBuf->readUint32(type);//参数变量类型

        container::AKBuf *buf=&script->mDataBuf;
        switch(type)
        {
            case AKVar::typeNumber:
                double num;
                buf->setPos(pos);
                buf->readDouble(num);
                out->setType(AKVar::typeNumber);
                out->setValue(num);
                break;
            case AKVar::typeString:
            {
                uint32 bytes;
                buf->setPos(pos);
                buf->readUint32(bytes);
                container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
                int8 *tmpStr=mem.alloc(bytes);
                buf->read(tmpStr,bytes);
                out->setType(AKVar::typeString);
                out->setValue(std::string(tmpStr,bytes));
                break;
            }
            case AKVar::typeHandle:
            {
                buf->setPos(pos);
                uint32 size=sizeof(void *);
                void *handle=NULL;
                buf->read((int8 *)&handle,size);
                out->setType(AKVar::typeHandle);
                out->setValue(handle);
                break;
            }
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //当前进程中，返回与上次dt调用的间隔时间（毫秒）
    bool AKHandleProcess::pDt(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double &mT0=script->getProcess()->mT0;
        double t1= AKCommon::time_milli();
        double dt=t1-mT0;
        mT0=t1;

        out->setValue((double)dt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断句柄是否为空
    bool AKHandleProcess::empty(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        void *handle=order->getParam(1)->toHandle();
        if(handle)out->setValue((double)0);
        else out->setValue((double)1);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除进程变量
    bool AKHandleProcess::pRemove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=&script->getProcess()->mProcessVarDict;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            dict->remove(index->toUint());
        }
        else if(index->getType()==AKVar::typeString)
        {
            dict->remove(index->toString());
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //进程变量是否存在
    bool AKHandleProcess::pExist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);

        container::AKDict *dict=&script->getProcess()->mProcessVarDict;
        AKVar *var=NULL;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            var=dict->get(index->toUint());
            if(var)
            {
                out->setValue((double)1);
            }
            else
            {
                out->setValue((double)0);
            }
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=dict->get(index->toString());
            if(var)
            {
                out->setValue((double)1);
            }
            else
            {
                out->setValue((double)0);
            }
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取进程变量引用
    bool AKHandleProcess::pGetRef(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        container::AKDict *dict=&script->getProcess()->mProcessVarDict;
        AKVar *var=NULL;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            var=dict->get(index->toUint());
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=dict->get(index->toString());
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        out->ref(var,true);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取进程变量值
    bool AKHandleProcess::pGet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKDict *dict=&script->getProcess()->mProcessVarDict;
        AKVar *var=NULL;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            var=dict->get(index->toUint());
            if(var)
            {
                out->setValue(var);
            }
            else
            {
                out->setType(AKVar::typeString);
                out->setValue("");
            }
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=dict->get(index->toString());
            if(var)
            {
                out->setValue(var);
            }
            else
            {
                out->setType(AKVar::typeString);
                out->setValue("");
            }
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置进程变量值
    bool AKHandleProcess::pSet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=&script->getProcess()->mProcessVarDict;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            dict->set(index->toUint(),order->getParam(2));
        }
        else if(index->getType()==AKVar::typeString)
        {
            dict->set(index->toString(),order->getParam(2));
        }
        else//非法变量
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取核数量
    bool AKHandleProcess::coreCount(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)script->getSystem()->getCoreCount()-1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取核id
    bool AKHandleProcess::coreID(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)script->getCore()->getID());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向进程pID回复数据
    bool AKHandleProcess::pResponse(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        //进程句柄
        AKProcess *process=(AKProcess *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkProcess(process))
        {
            onRunError(order,"无效进程pID");
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

        process->response(script->getProcess(),bufArray,bufCount);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //检查是否收到进程pID的msg请求数据
    bool AKHandleProcess::pHasRequest(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *pID=order->getParam(1);
        container::AKBuf *msgBuf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(msgBuf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        msgBuf->clear();
        AKProcess *process=process=script->getProcess()->hasRequest(*msgBuf);
        if(msgBuf->size()>0)//接收到msg
        {
            pID->setType(AKVar::typeHandle);
            pID->setValue((void *)process);
            msgBuf->setPos(0);
            out->setValue((double)1);
            script->nextOrder();//下一行指令
            return true;//继续执行脚本
        }
        else//没有接收到msg
        {
            out->setValue((double)0);
            script->nextOrder();//下一行指令
            return false;//暂停执行脚本
        }
    }

    //向进程pID请求数据，直到数据返回或超时为止
    bool AKHandleProcess::pRequest(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        switch(script->getOrderStatus())
        {
            case 0:
            {
                //进程句柄
                AKProcess *process=(AKProcess *)order->getParam(1)->toHandle();
                if(process==script->getProcess())
                {
                    onRunError(order,"不能向自身发起数据请求");
                    return false;//暂停执行脚本
                }
                if(!script->getCore()->checkProcess(process))
                {
                    onRunError(order,"无效进程pID");
                    return false;//暂停执行脚本
                }

                //超时时间（毫秒）
                uint32 timeout=order->getParam(2)->toUint();

                //重置接收计时器
                script->getProcess()->mTimer.restart();

                uint32 bufCount=order->varParams.size()-4;
                if(bufCount>10)
                {
                    onRunError(order,"最多连续发送10个数据buf");
                    return false;//暂停执行脚本
                }

                //接收返回数据的buf
                container::AKBuf *rtBuf=(container::AKBuf *)order->getParam(3)->toHandle();
                if(!script->getCore()->checkBuf(rtBuf))
                {
                    onRunError(order,"无效rtBuf");
                    return false;//暂停执行脚本
                }

                container::AKBuf *bufArray[10];
                container::AKBuf *buf=NULL;
                for(uint32 i=0;i<bufCount;++i)
                {
                    buf=(container::AKBuf *)order->getParam(i+4)->toHandle();
                    if(!script->getCore()->checkBuf(buf))
                    {
                        onRunError(order,"无效buf");
                        return false;//暂停执行脚本
                    }
                    bufArray[i]=buf;
                }

                process->request(script->getProcess(),bufArray,bufCount);
                script->setOrderStatus(1);
                break;
            }
            default:
            {
                //接收返回数据的buf
                container::AKBuf *rtBuf=(container::AKBuf *)order->getParam(3)->toHandle();

                if(script->getProcess()->getResponse(*rtBuf))//接收成功
                {
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                else
                {
                    //超时时间（毫秒）
                    uint32 timeout=order->getParam(2)->toUint();
                    if(timeout>0)
                    {
                        if((script->getProcess()->mTimer.elapsed()*1000)>=timeout)
                        {
                            out->setValue((double)0);
                            script->nextOrder();//下一行指令
                            return true;//继续执行脚本
                        }
                    }
                }
            }
        }
        return false;//暂停执行脚本
    }

    //绑定进程，被绑定的进程在当前进程死亡后也跟着死亡
    bool AKHandleProcess::pBind(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKProcess *process=(AKProcess *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkProcess(process))
        {
            onRunError(order,"无效进程pID");
            return false;//暂停执行脚本
        }

        script->getProcess()->bind(process);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //杀死进程
    bool AKHandleProcess::pKill(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKProcess *process=(AKProcess *)order->getParam(1)->toHandle();
        //if(!script->getCore()->checkProcess(process))
        //{
        //	onRunError(order,"无效进程pID");
        //	return false;//暂停执行脚本
        //}

        script->getSystem()->destroyProcess(process);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建空句柄
    bool AKHandleProcess::_handle(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        out->setValue((void *)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //接收进程pID发送的msg
    bool AKHandleProcess::pRecv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *pID=order->getParam(1);
        container::AKBuf *msgBuf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(msgBuf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        msgBuf->clear();
        AKProcess *process=script->getProcess()->recv(*msgBuf);
        if(msgBuf->size()>0)//接收到msg
        {
            pID->setType(AKVar::typeHandle);
            pID->setValue((void *)process);
            msgBuf->setPos(0);
            out->setValue((double)1);
            script->nextOrder();//下一行指令
            return true;//继续执行脚本
        }
        else//没有接收到msg
        {
            out->setValue((double)0);
            script->nextOrder();//下一行指令
            return false;//暂停执行脚本
        }
    }

    //向进程pID发送msg
    bool AKHandleProcess::pSend(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKProcess *process=(AKProcess *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkProcess(process))
        {
            onRunError(order,"无效进程pID");
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

        process->send(script->getProcess(),bufArray,bufCount);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //返回数据给发起exec的调用者
    bool AKHandleProcess::_return(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        script->mRtVar.setValue(order->getParam(1));

        out->setValue((double)1);
        script->stop();
        //return false;//暂停执行脚本
        //script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //调用脚本函数2（为了支持直接使用函数句柄变量名调用）
    bool AKHandleProcess::exec2(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->resetValue();
        try
        {
            switch(script->getOrderStatus())
            {
                case 0:
                {
                    ///////////////////////////////////////////////////////////////////////////////////////////////
                    AKVar *scriptFuncVar=order->scriptFuncVar;
                    if(scriptFuncVar)
                    {
                        if(scriptFuncVar->getType()==AKVar::typeString)//脚本函数url字符串存在
                        {
                            //根据字符串URL创建或找到脚本函数
                            std::string url=AKCommon::scriptUrlFormat(script,scriptFuncVar->toString());
                            const int8 *scriptUrl=url.c_str();
                            mScriptFunc=script->createScriptFunc(scriptUrl);
                        }
                        else if(scriptFuncVar->getType()==AKVar::typeHandle)//脚本函数句柄存在
                        {
                            mScriptFunc=(AKScript *)scriptFuncVar->toHandle();//取出指令绑定的脚本函数
                        }
                    }
                    else
                    {
                        //根据脚本API名，创建或找到脚本函数
                        AKStringStringMap &apiMap=AKGlobal::getSingleton().mAPIMap;
                        std::string url=AKCommon::scriptUrlFormat(script,apiMap[order->apiName].c_str());
                        const int8 *scriptUrl=url.c_str();
                        mScriptFunc=script->createScriptFunc(scriptUrl);
                    }
                    ///////////////////////////////////////////////////////////////////////////////////////////////
                    if(!script->checkScriptFuncCanExec(mScriptFunc))
                    {
                        onRunError(order,"同一个脚本函数实例不能被递归调用");
                        return false;//暂停执行脚本
                    }
                    if(!script->checkScriptFunc(mScriptFunc))
                    {
                        onRunError(order,"无效脚本函数句柄");
                        return false;//暂停执行脚本
                    }
                    mScriptFunc->quickReset();//快速重置

                    container::AKBuf *buf=&mScriptFunc->mDataBuf;
                    container::AKBuf *posBuf=&mScriptFunc->mDataPosBuf;
                    buf->clear();
                    posBuf->clear();
                    uint32 i=1;
                    AKVar *input=NULL;
                    while(input=order->getParam(i))
                    {
                        switch(input->getType())
                        {
                            case AKVar::typeNumber:
                                posBuf->writeUint32(buf->getPos());//写入变量在缓冲buf中的位置
                                posBuf->writeUint32(AKVar::typeNumber);//写入变量类型
                                buf->writeDouble(input->toNumber());
                                break;
                            case AKVar::typeString:
                            {
                                posBuf->writeUint32(buf->getPos());//写入变量在缓冲buf中的位置
                                posBuf->writeUint32(AKVar::typeString);//写入变量类型
                                uint32 bytes=input->toStringSize();
                                buf->writeUint32(bytes);//字符串占用字节数
                                buf->write(input->toString(),bytes);//字符串内容
                                break;
                            }
                            case AKVar::typeHandle:
                            {
                                posBuf->writeUint32(buf->getPos());//写入变量在缓冲buf中的位置
                                posBuf->writeUint32(AKVar::typeHandle);//写入变量类型
                                void *handle=input->toHandle();
                                buf->write((const int8 *)&handle,sizeof(void *));
                                break;
                            }
                            default://无效参数
                            {
                                int8 error[1024];
                                sprintf(error,"第%lu个参数无效",i);
                                onRunError(order,error);
                                return false;//暂停执行脚本
                            }
                        }
                        ++i;
                    }

                    if(mScriptFunc->run(0)==false)//脚本执行完成
                    {
                        out->clearType();
                        out->setValue(&mScriptFunc->mRtVar);
                        mScriptFunc->mRtVar.clearType();
                        mScriptFunc=NULL;
                        script->nextOrder();//下一行指令
                        return true;//继续执行脚本
                    }

                    script->setOrderStatus(1);
                    break;
                }
                default:
                {
                    if(mScriptFunc->run(0)==false)//脚本执行完成
                    {
                        out->clearType();
                        out->setValue(&mScriptFunc->mRtVar);
                        mScriptFunc->mRtVar.clearType();
                        mScriptFunc=NULL;
                        script->nextOrder();//下一行指令
                        return true;//继续执行脚本
                    }
                    break;
                }
            }
        }
        catch(const int8 *error)
        {
            onRunError(order,error);
            return false;//暂停执行脚本
        }
        return false;//暂停执行脚本
    }

    //创建脚本函数句柄
    bool AKHandleProcess::func(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        std::string url=AKCommon::scriptUrlFormat(script,order->getParam(1)->toString());
        const int8 *scriptUrl=url.c_str();
        AKScript *scriptFunc=script->createScriptFunc(scriptUrl);

        out->setValue((void *)scriptFunc);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取进程id
    bool AKHandleProcess::pID(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        out->setValue((void *)script->getProcess());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取系统预留核ID（用于系统任务）
    bool AKHandleProcess::sysCoreID(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)script->getSystem()->getCoreCount());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建进程
    bool AKHandleProcess::pCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        switch(script->getOrderStatus())
        {
            case 0:
            {
                std::string url=AKCommon::scriptUrlFormat(script,order->getParam(1)->toString());
                const int8 *scriptUrl=url.c_str();
                uint32 coreID=0;
                if(order->getParam(3))
                {
                    coreID=order->getParam(3)->toUint();
                    if(coreID>script->getSystem()->getCoreCount())
                    {
                        int8 error[1024];
                        sprintf(error,"核id %lu 不存在",coreID);
                        script->getSystem()->onRunError(error);
                        return false;//暂停执行脚本
                    }
                }
                container::AKBuf *buf=NULL;
                if(order->getParam(4))
                {
                    buf=(container::AKBuf *)order->getParam(4)->toHandle();
                }
                mProcess=script->getSystem()->createProcess(scriptUrl,coreID,buf);
                if(order->getParam(2))
                {
                    if(order->getParam(2)->toUint()!=0)//绑定进程
                    {
                        script->getProcess()->bind(mProcess);
                    }
                }
                script->setOrderStatus(1);
                break;
            }
            default://在下一个指令周期才执行下一条指令
                out->setValue((void *)mProcess);
                script->nextOrder();//下一行指令
                mProcess=NULL;
                return true;//继续执行脚本
                break;
        }
        return false;//暂停执行脚本
    }

    //创建进程（禁止进程内断点）
    bool AKHandleProcess::pCreateNoBreakPoint(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        switch(script->getOrderStatus())
        {
            case 0:
            {
                std::string url=AKCommon::scriptUrlFormat(script,order->getParam(1)->toString());
                const int8 *scriptUrl=url.c_str();
                uint32 coreID=0;
                if(order->getParam(3))
                {
                    coreID=order->getParam(3)->toUint();
                    if(coreID>script->getSystem()->getCoreCount())
                    {
                        int8 error[1024];
                        sprintf(error,"核id %lu 不存在",coreID);
                        script->getSystem()->onRunError(error);
                        return false;//暂停执行脚本
                    }
                }
                container::AKBuf *buf=NULL;
                if(order->getParam(4))
                {
                    buf=(container::AKBuf *)order->getParam(4)->toHandle();
                }
                mProcess=script->getSystem()->createProcess(scriptUrl,coreID,buf);
                mProcess->mIsNoBreakPoint=true;
                if(order->getParam(2))
                {
                    if(order->getParam(2)->toUint()!=0)//绑定进程
                    {
                        script->getProcess()->bind(mProcess);
                    }
                }
                script->setOrderStatus(1);
                break;
            }
            default://在下一个指令周期才执行下一条指令
                out->setValue((void *)mProcess);
                script->nextOrder();//下一行指令
                mProcess=NULL;
                return true;//继续执行脚本
                break;
        }
        return false;//暂停执行脚本
    }

    //获取进程脚本url
    bool AKHandleProcess::pScriptUrl(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKProcess *process=script->getProcess();
        if(process)
        {
            out->setValue((int8 *)process->mScript.getUrl());
        }
        else
        {
            out->setValue((int8 *)"");
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
