#include "ak.h"
#include "AKScript.h"
#include "AKHandleBuf.h"
#include "AKGlobal.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleBuf::AKHandleBuf()
            :mParent(NULL)
    {
    }

    AKHandleBuf::~AKHandleBuf()
    {
        reset();
    }

    void AKHandleBuf::reset()
    {
        AKBufMap::iterator it=mBufMap.begin();
        while(it!=mBufMap.end())
        {
            delete it->second;
            ++it;
        }
        mBufMap.clear();
    }

    void AKHandleBuf::init(AKScript *script)
    {
        AKHandler::init(script);

        //缓冲API
        AKQuickBindScriptFunc("buf",&AKHandleBuf::bufCreate,"buf=buf();buf=buf(str);//创建缓冲");
        AKQuickBindScriptFunc("bufCreate",&AKHandleBuf::bufCreate,"//同buf()，已淘汰");
        AKQuickBindScriptFunc("gBufCreate",&AKHandleBuf::bufCreate,"//同buf()，已淘汰");
        AKQuickBindScriptFunc("pBufCreate",&AKHandleBuf::bufCreate,"//同buf()，已淘汰");
        AKQuickBindScriptFunc("bufDestroy",&AKHandleBuf::bufDestroy,"bufDestroy(buf);//销毁缓冲");
        AKQuickBindScriptFunc("isBuf",&AKHandleBuf::isBuf,"isBuf=isBuf(var);//判断是否为缓冲");

        AKQuickBindScriptFunc("bufGetScriptData",&AKHandleBuf::bufGetScriptData,"buf=bufGetScriptData();//获取异步脚本数据缓冲");
        AKQuickBindScriptFunc("in",&AKHandleBuf::bufGetScriptData,"buf=in();//获取异步脚本数据缓冲");
        AKQuickBindScriptFunc("bufAppend",&AKHandleBuf::bufAppend,"bufAppend(destBuf,srcBuf[,offset,length]);//合并缓冲");
        AKQuickBindScriptFunc("bufSize",&AKHandleBuf::bufSize,"size=bufSize(buf);//获取缓冲字节数");
        AKQuickBindScriptFunc("bufClear",&AKHandleBuf::bufClear,"buf=bufClear(buf);//清空缓冲");
        AKQuickBindScriptFunc("bufSetPos",&AKHandleBuf::bufSetPos,"bufSetPos(buf,pos);//设置缓冲位置（有可能增大缓冲大小）");
        AKQuickBindScriptFunc("bufOffset",&AKHandleBuf::bufOffset,"bufOffset(buf,pos);//设置缓冲位置（不影响缓冲大小）");
        AKQuickBindScriptFunc("bufGetPos",&AKHandleBuf::bufGetPos,"pos=bufGetPos(buf);//获取缓冲位置");
        AKQuickBindScriptFunc("bufSetEnd",&AKHandleBuf::bufSetEnd,"pos=bufSetEnd(buf);//设置缓冲位置到尾部");

        AKQuickBindScriptFunc("buf2hex",&AKHandleBuf::buf2hex,"hexStr=buf2hex(buf);//缓冲内容转hex十六进制字符串");
        AKQuickBindScriptFunc("hex2buf",&AKHandleBuf::hex2buf,"hex2buf(buf,hexStr);//hex十六进制字符串转缓冲内容");

        AKQuickBindScriptFunc("packStr",&AKHandleBuf::packStr,"buf=packStr(buf,str);//打包字符串到缓冲");
        AKQuickBindScriptFunc("bufFormat",&AKHandleBuf::bufFormat,"buf=bufFormat(buf,10001,\"abc\",pID,buf,arr,dict,...);//缓冲格式化连续输入多个变量，变量可以是数值、字符串、句柄，输入字符串时将自动输入字符串占用字节数数值和字符串内容");
        AKQuickBindScriptFunc("pack",&AKHandleBuf::bufFormat,"buf=pack(buf,10001,\"abc\",pID,buf,arr,dict,...);//同bufFormat");

        AKQuickBindScriptFunc("bufWriteUint8",&AKHandleBuf::bufWriteUint8,"bufWriteUint8(buf,num);//向缓冲写入uint8");
        AKQuickBindScriptFunc("bufReadUint8",&AKHandleBuf::bufReadUint8,"num=bufReadUint8(buf);//从缓冲读出uint8");

        AKQuickBindScriptFunc("bufWriteInt8",&AKHandleBuf::bufWriteInt8,"bufWriteInt8(buf,num);//向缓冲写入int8");
        AKQuickBindScriptFunc("bufReadInt8",&AKHandleBuf::bufReadInt8,"num=bufReadInt8(buf);//从缓冲读出int8");

        AKQuickBindScriptFunc("bufWriteUint16",&AKHandleBuf::bufWriteUint16,"bufWriteUint16(buf,num);//向缓冲写入uint16");
        AKQuickBindScriptFunc("bufReadUint16",&AKHandleBuf::bufReadUint16,"num=bufReadUint16(buf);//从缓冲读出uint16");

        AKQuickBindScriptFunc("bufWriteInt16",&AKHandleBuf::bufWriteInt16,"bufWriteInt16(buf,num);//向缓冲写入int16");
        AKQuickBindScriptFunc("bufReadInt16",&AKHandleBuf::bufReadInt16,"num=bufReadInt16(buf);//从缓冲读出int16");

        AKQuickBindScriptFunc("bufWriteUint32",&AKHandleBuf::bufWriteUint32,"bufWriteUint32(buf,num);//向缓冲写入uint32");
        AKQuickBindScriptFunc("bufReadUint32",&AKHandleBuf::bufReadUint32,"num=bufReadUint32(buf);//从缓冲读出uint32");

        AKQuickBindScriptFunc("bufWriteInt32",&AKHandleBuf::bufWriteInt32,"bufWriteInt32(buf,num);//向缓冲写入int32");
        AKQuickBindScriptFunc("bufReadInt32",&AKHandleBuf::bufReadInt32,"num=bufReadInt32(buf);//从缓冲读出int32");

        AKQuickBindScriptFunc("bufWriteFloat",&AKHandleBuf::bufWriteFloat,"bufWriteFloat(buf,num);//向缓冲写入float");
        AKQuickBindScriptFunc("bufReadFloat",&AKHandleBuf::bufReadFloat,"num=bufReadFloat(buf);//从缓冲读出float");

        AKQuickBindScriptFunc("bufWriteDouble",&AKHandleBuf::bufWriteDouble,"bufWriteDouble(buf,num);//向缓冲写入double");
        AKQuickBindScriptFunc("bufReadDouble",&AKHandleBuf::bufReadDouble,"num=bufReadDouble(buf);//从缓冲读出double");

        AKQuickBindScriptFunc("bufWriteString",&AKHandleBuf::bufWriteString,"bufWriteString(buf,str[,size]);//向缓冲写入string");
        AKQuickBindScriptFunc("bufReadString",&AKHandleBuf::bufReadString,"str=bufReadString(buf,size);//从缓冲读出string");

        AKQuickBindScriptFunc("bufWriteHandle",&AKHandleBuf::bufWriteHandle,"bufWriteHandle(buf,handle);//向缓冲写入句柄");
        AKQuickBindScriptFunc("bufReadHandle",&AKHandleBuf::bufReadHandle,"handle=bufReadHandle(buf);//从缓冲读出句柄");

        AKQuickBindScriptFunc("bufWriteUint32String",&AKHandleBuf::bufWriteUint32String,"bufWriteUint32String(buf,str);//向缓冲写入uint32和strint，其中uint32为string所占字节数，包含字符串结束符");
        AKQuickBindScriptFunc("bufReadUint32String",&AKHandleBuf::bufReadUint32String,"str=bufReadUint32String(buf);//从缓冲先读出uint32，然后根据uint32读出string");

        //序列化API
        AKQuickBindScriptFunc("serialize",&AKHandleBuf::serialize,"buf=serialize(var);//序列化到缓冲//var可以是变量或嵌套的容器");
        AKQuickBindScriptFunc("unserialize",&AKHandleBuf::unserialize,"var=unserialize(buf);//反序列化到变量");

        //字符串和缓冲的转换API
        AKQuickBindScriptFunc("buf2str",&AKHandleBuf::buf2str,"str=buf2str(buf);//缓冲转字符串");
        AKQuickBindScriptFunc("str2buf",&AKHandleBuf::str2buf,"buf=str2buf(str);//字符串转缓冲");

        //结构体 api
        AKQuickBindScriptFunc("structMgr_create",&AKHandleBuf::structMgr_create,"mgr=structMgr_create();//创建结构体管理器");
        AKQuickBindScriptFunc("structMgr_destroy",&AKHandleBuf::structMgr_destroy,"structMgr_destroy(mgr);//销毁结构体管理器");
        AKQuickBindScriptFunc("structMgr_load",&AKHandleBuf::structMgr_load,"structMgr_load(mgr,structStr);//加载结构体定义字符串");
        AKQuickBindScriptFunc("structMgr_upgradeObj",&AKHandleBuf::structMgr_upgradeObj,"obj=structMgr_upgradeObj(mgr,structName,obj);//根据结构体定义升级obj成员");
        AKQuickBindScriptFunc("structMgr_getNameArr",&AKHandleBuf::structMgr_getNameArr,"nameArr=structMgr_getNameArr(mgr,structName,nameArr);//获取结构体成员名称数组，structName为空时，代表返回所有结构体名称");

    }

    //获取结构体成员名称数组，structName为空时，代表返回所有结构体名称
    bool AKHandleBuf::structMgr_getNameArr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKStruct *structObj=(container::AKStruct *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkStruct(structObj))
        {
            onRunError(order,"无效structMgr");
            return false;//暂停执行脚本
        }

        const int8 *structName=order->getParam(2)->toString();

        container::AKArr *arr=(container::AKArr *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }
        arr->clear();

        if(strcmp(structName,"")==0)//返回所有结构体名称
        {
            container::AKStruct::StructMap &structMap=structObj->structMap;
            container::AKStruct::StructMap::iterator it=structMap.begin();
            while(it!=structMap.end())
            {
                const std::string &name=it->first;
                arr->push(name.c_str());
                ++it;
            }
        }
        else
        {
            container::AKStruct::StructMap::iterator it=structObj->structMap.find(structName);
            if(it!=structObj->structMap.end())
            {
                container::AKStruct::Struct &curStruct=it->second;
                container::AKStruct::Struct::MemberMap &memberMap=curStruct.memberMap;
                container::AKStruct::Struct::MemberMap::iterator it2=memberMap.begin();
                while(it2!=memberMap.end())
                {
                    container::AKStruct::Member &member=it2->second;
                    arr->push(member.name.c_str());
                    arr->push(member.type);
                    arr->push(member.val.c_str());
                    ++it2;
                }
            }
        }


        out->setValue((void*)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //根据结构体定义升级obj成员
    bool AKHandleBuf::structMgr_upgradeObj(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKStruct *structObj=(container::AKStruct *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkStruct(structObj))
        {
            onRunError(order,"无效structMgr");
            return false;//暂停执行脚本
        }

        //结构体名称
        const int8 *structName=order->getParam(2)->toString();

        //需要升级的对象
        AKVar *var=order->getParam(3);

        //升级
        structObj->upgradeObj(script->getCore(),var,structName);

        out->setValue(var->toHandle());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //销毁结构体管理器
    bool AKHandleBuf::structMgr_load(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKStruct *structObj=(container::AKStruct *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkStruct(structObj))
        {
            onRunError(order,"无效structMgr");
            return false;//暂停执行脚本
        }

        const int8 *structStr=order->getParam(2)->toString();

        uint32 pos=0;
        bool rt=structObj->loadStruct(structStr,pos);
        if(!rt)
        {
            //查找错误行号
            uint32 rows=1;
            for(uint32 i=0;i<=pos;++i)
            {
                if(structStr[i]=='\n')++rows;
            }
            int8 tmp[1024];
            sprintf(tmp,"structMgr_load error at row %lu",rows);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        else
        {
            out->setValue((double)0);
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //销毁结构体管理器
    bool AKHandleBuf::structMgr_destroy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKStruct *structObj=(container::AKStruct *)order->getParam(1)->toHandle();
        script->getCore()->destroyStruct(structObj);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建结构体管理器
    bool AKHandleBuf::structMgr_create(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKStruct *structObj=script->getCore()->createStruct();

        out->setValue((void *)structObj);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //缓冲转字符串
    bool AKHandleBuf::buf2str(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 bytes=buf->size();
        container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
        int8 *tmp=mem.alloc(bytes);

        uint32 oldPos=buf->getPos();
        buf->setPos(0);
        buf->read(tmp,bytes);
        buf->setPos(oldPos);

        std::string str(tmp,bytes);

        out->setValue(str);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符串转缓冲
    bool AKHandleBuf::str2buf(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        std::string &str=order->getParam(1)->toStdString();

        container::AKBuf *buf=script->getCore()->createBuf();
        buf->write(str.c_str(),str.size());

        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断是否为缓冲
    bool AKHandleBuf::isBuf(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        bool isBuf=false;
        if(order->getParam(1)->getType()==AKVar::typeHandle)
        {
            container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
            if(script->getCore()->checkBuf(buf))isBuf=true;
        }

        if(isBuf)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //反序列化到变量
    bool AKHandleBuf::unserialize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 oldPos=buf->getPos();
        buf->setPos(0);
        bool rt=AKCommon::unserialize(script->getCore(),out,buf);
        if(!rt)
        {
            onRunError(order,"反序列化失败");
            return false;//暂停执行脚本
        }
        buf->setPos(oldPos);

        //out->setValue((void *)rtBuf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //序列化到缓冲//var可以是变量或嵌套的容器
    bool AKHandleBuf::serialize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *rtBuf=script->getCore()->createBuf();//返回的缓冲

        AKVar *var=order->getParam(1);//需要序列化的变量或嵌套容器

        std::string error;
        bool rt=AKCommon::serialize(script->getCore(),rtBuf,var,error);
        if(!rt)
        {
            onRunError(order,error.c_str());
            return false;//暂停执行脚本
        }

        out->setValue((void *)rtBuf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //hex十六进制字符串转缓冲内容
    bool AKHandleBuf::hex2buf(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        std::string hexStr=order->getParam(2)->toString();
        if(!AKCommon::hex2buf(buf,hexStr))
        {
            onRunError(order,"无效hexStr");
            return false;//暂停执行脚本
        }

        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //缓冲内容转hex十六进制字符串
    bool AKHandleBuf::buf2hex(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        std::string hexStr;
        AKCommon::buf2hex(hexStr,buf);

        out->setValue(hexStr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //打包字符串到缓冲
    bool AKHandleBuf::packStr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        std::string &str=order->getParam(2)->toStdString();

        buf->clear();
        buf->write(str.c_str(),str.size());

        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲先读出uint32，然后根据uint32读出string
    bool AKHandleBuf::bufReadUint32String(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 bytes=0;
        if(!buf->readUint32(bytes))
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
        int8 *tmp=mem.alloc(bytes);
        bool rt=buf->read(tmp,bytes);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }
        out->setValue(std::string(tmp,bytes));

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入uint32和strint，其中uint32为string所占字节数，包含字符串结束符
    bool AKHandleBuf::bufWriteUint32String(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        std::string &str=order->getParam(2)->toStdString();
        uint32 bytes=str.size();//占用字节数

        buf->writeUint32(bytes);
        buf->write(str.c_str(),bytes);
        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //缓冲格式化连续输入多个变量，变量可以是数值、字符串、句柄，输入字符串时将自动输入字符串占用字节数数值和字符串内容
    bool AKHandleBuf::bufFormat(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        buf->clear();

        uint32 i=2;
        AKVar *input=NULL;
        while(input=order->getParam(i))
        {
            switch(input->getType())
            {
                case AKVar::typeNumber:
                    buf->writeDouble(input->toNumber());
                    break;
                case AKVar::typeString:
                {
                    uint32 bytes=input->toStringSize();
                    buf->writeUint32(bytes);//字符串占用字节数
                    buf->write(input->toString(),bytes);//字符串内容
                    break;
                }
                case AKVar::typeHandle:
                {
                    void *handle=input->toHandle();
                    buf->write((const int8 *)&handle,sizeof(void *));
                    break;
                }
            }
            ++i;
        }

        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取异步脚本数据缓冲
    bool AKHandleBuf::bufGetScriptData(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=script->getCore()->createBuf();
        buf->append(script->mDataBuf);//复制数据副本
        buf->setPos(0);

        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //销毁缓冲
    bool AKHandleBuf::bufDestroy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        script->getCore()->destroyRef(buf);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建临时缓冲
    bool AKHandleBuf::bufCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=script->getCore()->createBuf();

        if(order->getParam(1))
        {
            std::string &str=order->getParam(1)->toStdString();
            buf->write(str.c_str(),str.size());
        }

        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //在缓冲当前位置附加数据
    bool AKHandleBuf::bufAppend(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *bufDest=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(bufDest))
        {
            onRunError(order,"无效目标buf");
            return false;//暂停执行脚本
        }

        container::AKBuf *bufSrc=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(bufSrc))
        {
            onRunError(order,"无效源buf");
            return false;//暂停执行脚本
        }

        uint32 offset=0;
        if(order->getParam(3))offset=order->getParam(3)->toUint();

        uint32 size=0;
        if(order->getParam(4))size=order->getParam(4)->toUint();

        bufDest->append(*bufSrc,offset,size);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取缓冲字节数
    bool AKHandleBuf::bufSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        out->setValue((double)buf->size());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //清空缓冲
    bool AKHandleBuf::bufClear(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        buf->clear();
        out->setValue((void *)buf);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置缓冲位置到尾部
    bool AKHandleBuf::bufSetEnd(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        buf->setEnd();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置缓冲位置
    bool AKHandleBuf::bufOffset(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        buf->offset(order->getParam(2)->toUint());

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置缓冲位置
    bool AKHandleBuf::bufSetPos(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        buf->setPos(order->getParam(2)->toUint());

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取缓冲位置
    bool AKHandleBuf::bufGetPos(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        out->setValue((double)buf->getPos());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出string
    bool AKHandleBuf::bufReadHandle(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 size=sizeof(void *);
        void *handle=NULL;
        bool rt=buf->read((int8 *)&handle,size);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }
        out->setValue(handle);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入string
    bool AKHandleBuf::bufWriteHandle(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 size=sizeof(void *);
        void *handle=order->getParam(2)->toHandle();
        buf->write((const int8 *)&handle,size);
        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出string
    bool AKHandleBuf::bufReadString(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 size=order->getParam(2)->toUint();
        container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
        int8 *tmp=mem.alloc(size);
        bool rt=buf->read(tmp,size);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue(std::string(tmp,size));

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入string
    bool AKHandleBuf::bufWriteString(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        std::string &str=order->getParam(2)->toStdString();

        uint32 size=0;
        if(order->getParam(3))
        {
            size=order->getParam(3)->toUint();
        }
        if(size==0)
        {
            size=str.size();
        }
        buf->write(str.c_str(),size);
        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出double
    bool AKHandleBuf::bufReadDouble(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        double num;
        bool rt=buf->readDouble(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入double
    bool AKHandleBuf::bufWriteDouble(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        double num=order->getParam(2)->toNumber();
        buf->writeDouble(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出float
    bool AKHandleBuf::bufReadFloat(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        float num;
        bool rt=buf->readFloat(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入float
    bool AKHandleBuf::bufWriteFloat(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        float num=order->getParam(2)->toNumber();
        buf->writeFloat(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出uint32
    bool AKHandleBuf::bufReadUint32(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 num;
        bool rt=buf->readUint32(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入uint32
    bool AKHandleBuf::bufWriteUint32(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint32 num=order->getParam(2)->toNumber();
        buf->writeUint32(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出int32
    bool AKHandleBuf::bufReadInt32(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        int32 num;
        bool rt=buf->readInt32(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入int32
    bool AKHandleBuf::bufWriteInt32(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        int32 num=order->getParam(2)->toNumber();
        buf->writeInt32(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出uint16
    bool AKHandleBuf::bufReadUint16(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint16 num;
        bool rt=buf->readUint16(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入uint16
    bool AKHandleBuf::bufWriteUint16(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint16 num=order->getParam(2)->toNumber();
        buf->writeUint16(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出int16
    bool AKHandleBuf::bufReadInt16(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        int16 num;
        bool rt=buf->readInt16(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入int16
    bool AKHandleBuf::bufWriteInt16(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        int16 num=order->getParam(2)->toNumber();
        buf->writeInt16(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出uint8
    bool AKHandleBuf::bufReadUint8(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint8 num;
        bool rt=buf->readUint8(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入uint8
    bool AKHandleBuf::bufWriteUint8(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        uint8 num=order->getParam(2)->toNumber();
        buf->writeUint8(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从缓冲读出int8
    bool AKHandleBuf::bufReadInt8(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        int8 num;
        bool rt=buf->readInt8(num);
        if(!rt)
        {
            onRunError(order,"读取buf超出范围");
            return false;//暂停执行脚本
        }

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向缓冲写入int8
    bool AKHandleBuf::bufWriteInt8(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        int8 num=order->getParam(2)->toNumber();
        buf->writeInt8(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
