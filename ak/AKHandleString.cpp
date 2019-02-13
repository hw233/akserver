#include "ak.h"
#include "AKHandleString.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleString::AKHandleString()
    {
    }

    AKHandleString::~AKHandleString()
    {
    }

    void AKHandleString::init(AKScript *script)
    {
        AKHandler::init(script);

        AKQuickBindScriptFunc("tolower",&AKHandleString::tolower,"newStr=tolower(str);//字符串转小写");
        AKQuickBindScriptFunc("toupper",&AKHandleString::toupper,"newStr=toupper(str);//字符串转大写");
        AKQuickBindScriptFunc("ord",&AKHandleString::ord,"ascii=ord(str);//str首字符转ascii码");
        AKQuickBindScriptFunc("ascii",&AKHandleString::ord,"ascii=ascii(str);//str首字符转ascii码");
        AKQuickBindScriptFunc("chr",&AKHandleString::chr,"ch=chr(ascii);//ascii码转字符");
        AKQuickBindScriptFunc("strsub",&AKHandleString::_strsub,"newStr=strsub(str,startPos[,len]);//截取字符串内容");
        AKQuickBindScriptFunc("strlen",&AKHandleString::_strlen,"len=strlen(str);//获取字符串字节数");
        AKQuickBindScriptFunc("string",&AKHandleString::_string,"str=string(num);//转换成字符串");
        AKQuickBindScriptFunc("strFind",&AKHandleString::strFind,"pos=strFind(srcStr,findStr[,beginPos]);//字符串查找,失败返回-1");
        AKQuickBindScriptFunc("strReplace",&AKHandleString::strReplace,"strReplace(srcStr,findStr,replaceStr);//字符串替换");
        AKQuickBindScriptFunc("strRepeat",&AKHandleString::strRepeat,"strRepeat(str,repeatCount);//字符串重复");
        AKQuickBindScriptFunc("str_c_escape",&AKHandleString::str_c_escape, "destStr=str_c_escape(srcStr);//C风格字符串转义");
        AKQuickBindScriptFunc("str_c_unescape",&AKHandleString::str_c_unescape, "destStr=str_c_unescape(srcStr);//C风格字符串逆转义");
        AKQuickBindScriptFunc("sprintf",&AKHandleString::str_sprintf, "str=sprintf(\"a=%0;b=%1;c=%2;\",val0,val1,val2);//字符串格式化");
        AKQuickBindScriptFunc("strtol",&AKHandleString::_strtol,"num=strtol(str,base);//字符串转数值，其中base是进制，例如16");
    }

    //字符串转数值，其中base是进制，例如16
    bool AKHandleString::_strtol(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *str=order->getParam(1)->toString();//字符串
        uint32 base=order->getParam(2)->toNumber();
        int8 *t;
        double num=strtol(str,&t,base);

        out->setValue(num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //C风格字符串转义
    bool AKHandleString::str_c_escape(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string srcStr=order->getParam(1)->toStdString();//源字符串
        AKCommon::str_c_escape(srcStr);

        out->setValue(srcStr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //C风格字符串逆转义
    bool AKHandleString::str_c_unescape(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string srcStr=order->getParam(1)->toStdString();//源字符串
        AKCommon::str_c_unescape(srcStr);

        out->setValue(srcStr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符串重复
    bool AKHandleString::strRepeat(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        const int8 *str=order->getParam(1)->toString();//字符串
        int32 repeatCount=order->getParam(2)->toInt();//重复次数
        if(repeatCount<0)repeatCount=0;

        std::string rt;
        for(int32 i=0;i<repeatCount;++i)
        {
            rt+=str;
        }

        out->setValue(rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符串格式化
    bool AKHandleString::str_sprintf(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string str=order->getParam(1)->toString();//需要格式化的字符串
        int8 tmp[50];
        for(int32 i=0;;++i)
        {
            AKVar *val=order->getParam(i+2);
            if(!val)break;
            sprintf(tmp,"%%%lu",i);
            AKCommon::strReplace(str,tmp,val->toString());
        }

        out->setValue(str.c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符串替换
    bool AKHandleString::strReplace(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        std::string srcStr=order->getParam(1)->toString();//源字符串
        std::string findStr=order->getParam(2)->toString();//查找的字符串
        std::string replaceStr=order->getParam(3)->toString();//替换的字符串

        AKCommon::strReplace(srcStr,findStr,replaceStr);
        order->getParam(1)->setValue((const int8 *)srcStr.c_str());

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }


    //字符串查找
    bool AKHandleString::strFind(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        std::string srcStr=order->getParam(1)->toString();//源字符串
        const int8 *findStr=order->getParam(2)->toString();//查找的字符串

        //截取开始位置
        int32 startPos=0;
        if(order->getParam(3))startPos=order->getParam(3)->toInt();

        //查找//失败返回-1
        int32 pos=srcStr.find(findStr,startPos);

        out->setValue((double)pos);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //转大写
    bool AKHandleString::toupper(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string str=order->getParam(1)->toString();
        std::transform(str.begin(),str.end(),str.begin(),::toupper);

        out->setValue(str);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //转小写
    bool AKHandleString::tolower(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string str=order->getParam(1)->toString();
        std::transform(str.begin(),str.end(),str.begin(),::tolower);

        out->setValue(str);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符转ascii码
    bool AKHandleString::ord(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *str=order->getParam(1)->toString();
        uint32 ascii=str[0];

        out->setValue((double)ascii);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //ascii码转字符
    bool AKHandleString::chr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        uint32 ascii=order->getParam(1)->toUint();//ascii码
        int8 ch=ascii;
        out->setValue(std::string(&ch,1));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //截取字符串内容
    bool AKHandleString::_strsub(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string &str=order->getParam(1)->toStdString();//源字符串
        int32 startPos=order->getParam(2)->toInt();//截取开始位置
        int32 len=-1;//截取长度
        if(order->getParam(3))len=order->getParam(3)->toInt();

        int32 s=str.size();
        if(startPos>=s||(startPos+len-1)>=s)
        {
            onRunError(order,"超出截取范围");
            return false;//暂停执行脚本
        }

        out->setValue(str.substr(startPos,len).c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字符串字节数
    bool AKHandleString::_strlen(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        uint32 len=order->getParam(1)->toStringSize();
        out->setValue((double)len);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //转换成字符串
    bool AKHandleString::_string(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKVar *var=order->getParam(1);
        if(var->getType()==AKVar::typeHandle)
        {
            uint64 handle=(uint64)var->toHandle();
            int8 tmp[1024];
            sprintf(tmp,"handle[%llu]",handle);
            out->setValue((const int8 *)tmp);
        }
        else
        {
            out->setValue(var->toString());
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
