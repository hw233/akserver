#include "ak.h"
#include "AKScript.h"
#include "AKHandleMath.h"
#include "AKHandleBuf.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleMath::AKHandleMath()
            :mHandleBuf(NULL)
    {
    }

    AKHandleMath::~AKHandleMath()
    {
    }

    AKHandleBuf *AKHandleMath::getHandleBuf()
    {
        if(!mHandleBuf)
        {
            mHandleBuf=(AKHandleBuf *)getScript()->getHandler("AKHandleBuf");
        }
        return mHandleBuf;
    }

    void AKHandleMath::init(AKScript *script)
    {
        AKHandler::init(script);

        AKQuickBindScriptFunc("base64",&AKHandleMath::base64,"rtStr=base64(str);//base64编码字符串");
        AKQuickBindScriptFunc("base64_encode",&AKHandleMath::base64,"rtStr=base64_encode(str);//base64编码字符串");
        AKQuickBindScriptFunc("unbase64",&AKHandleMath::unbase64,"rtStr=unbase64(str);//base64解码字符串");
        AKQuickBindScriptFunc("base64_decode",&AKHandleMath::unbase64,"rtStr=base64_decode(str);//base64解码字符串");
        AKQuickBindScriptFunc("urlencode",&AKHandleMath::urlencode,"rtStr=urlencode(str);//url编码字符串");
        AKQuickBindScriptFunc("urldecode",&AKHandleMath::urldecode,"rtStr=urldecode(str);//url解码字符串");
        AKQuickBindScriptFunc("sha1",&AKHandleMath::sha1,"rtStr=sha1(str);rtStr=sha1(buf);//转为sha1字符串");
        AKQuickBindScriptFunc("rand",&AKHandleMath::order_Random,"num=rand();//随机函数");
        AKQuickBindScriptFunc("rand_max",&AKHandleMath::order_rand_max,"num=rand_max();//随机数最大值");
        AKQuickBindScriptFunc("srand",&AKHandleMath::order_srand,"srand(num);//设置随机种子");
        AKQuickBindScriptFunc("round",&AKHandleMath::order_Round,"num=round(3.14);//取整（四舍五入）");
        AKQuickBindScriptFunc("int",&AKHandleMath::order_Int,"num=int(3.14);//取整");
        AKQuickBindScriptFunc("number",&AKHandleMath::order_Number,"num=number(str);//转换成数值");
        AKQuickBindScriptFunc("sin",&AKHandleMath::order_Sin,"num=sin(a);//三角函数sin");
        AKQuickBindScriptFunc("asin",&AKHandleMath::order_Asin,"num=asin(a);//三角函数asin");
        AKQuickBindScriptFunc("cos",&AKHandleMath::order_Cos,"num=cos(a);//三角函数cos");
        AKQuickBindScriptFunc("acos",&AKHandleMath::order_Acos,"num=acos(a);//三角函数acos");
        AKQuickBindScriptFunc("tan",&AKHandleMath::order_Tan,"num=tan(a);//三角函数tan");
        AKQuickBindScriptFunc("atan",&AKHandleMath::order_Atan,"num=atan(a);//三角函数atan");
        AKQuickBindScriptFunc("pow",&AKHandleMath::order_Pow,"num=pow(a,n);//a的n次方");
        AKQuickBindScriptFunc("sqrt",&AKHandleMath::order_Sqrt,"num=sqrt(a);//a的开方");
        AKQuickBindScriptFunc("md5",&AKHandleMath::md5,"md5Str=md5(str);md5Str=md5(buf);//转为md5字符串");
        AKQuickBindScriptFunc("max",&AKHandleMath::order_max,"maxNum=max(num1,num2,num3,...);//获取最大值");
        AKQuickBindScriptFunc("min",&AKHandleMath::order_min,"minNum=min(num1,num2,num3,...);//获取最小值");
        AKQuickBindScriptFunc("limit",&AKHandleMath::order_limit,"destNum=limit(num,minNum,maxNum);//数值限制在minNum和maxNum的闭区间内");
        AKQuickBindScriptFunc("abs",&AKHandleMath::order_abs,"destNum=abs(num);//绝对值");
        AKQuickBindScriptFunc("ceil",&AKHandleMath::order_ceil,"destNum=ceil(num);//向上取整");
        AKQuickBindScriptFunc("floor",&AKHandleMath::order_floor,"destNum=floor(num);//向下取整");
        AKQuickBindScriptFunc("crc32",&AKHandleMath::crc32,"num=crc32(str);//crc 32位求和校验");
        AKQuickBindScriptFunc("uuid",&AKHandleMath::uuid,"uuid=uuid();//生成uuid");
        AKQuickBindScriptFunc("log10",&AKHandleMath::log10,"n=log10(a);//计算以10为底的a的对数");

        //向量api
        AKQuickBindScriptFunc("moveTo",&AKHandleMath::moveTo,"moveTo(x,y,xDest,yDest,speed,dt);//计算直线移动时的位置");
        AKQuickBindScriptFunc("vectLength",&AKHandleMath::vectLength,"len=vectLength(x0,y0,x1,y1);//计算向量长度");
        AKQuickBindScriptFunc("len",&AKHandleMath::vectLength,"len=len(x0,y0,x1,y1);//计算向量长度");
        AKQuickBindScriptFunc("vectAngle",&AKHandleMath::vectAngle,"angle=vectAngle(x0,y0,x1,y1);//计算向量角度");
        AKQuickBindScriptFunc("angle2radian",&AKHandleMath::angle2radian,"radian=angle2radian(angle);//角度转弧度");
        AKQuickBindScriptFunc("radian2angle",&AKHandleMath::radian2angle,"angle=radian2angle(radian);//弧度转角度");
    }

    //url编码字符串
    bool AKHandleMath::urlencode(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string rtStr=AKCommon::urlencode(order->getParam(1)->toStdString());

        out->setValue(rtStr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //url解码字符串
    bool AKHandleMath::urldecode(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string rtStr=AKCommon::urldecode(order->getParam(1)->toStdString());

        out->setValue(rtStr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //生成uuid
    bool AKHandleMath::uuid(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        out->setValue(AKCommon::uuid().c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //计算以10为底的a的对数
    bool AKHandleMath::log10(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double a=order->getParam(1)->toNumber();
        double n=::log10(a);

        out->setValue(n);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //crc 32位求和校验
    bool AKHandleMath::crc32(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)AKCommon::crc32(order->getParam(1)->toString(),order->getParam(1)->toStringSize()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向上取整
    bool AKHandleMath::order_ceil(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)ceil(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向下取整
    bool AKHandleMath::order_floor(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)floor(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    bool AKHandleMath::moveTo(AKScript *script,AKOrder *order)
    {
        if(!checkParams(6,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double x=order->getParam(1)->toNumber();
        double y=order->getParam(2)->toNumber();
        double xDest=order->getParam(3)->toNumber();
        double yDest=order->getParam(4)->toNumber();
        double speed=order->getParam(5)->toNumber();
        double dt=order->getParam(6)->toNumber();

        AKCommon::moveTo(x,y,xDest,yDest,speed,dt);
        order->getParam(1)->setValue((double)x);
        order->getParam(2)->setValue((double)y);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //弧度转角度
    bool AKHandleMath::radian2angle(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double angle=order->getParam(1)->toNumber();

        out->setValue((double)AKCommon::radian2angle(angle));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //角度转弧度
    bool AKHandleMath::angle2radian(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double angle=order->getParam(1)->toNumber();

        out->setValue((double)AKCommon::angle2radian(angle));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //计算向量角度
    bool AKHandleMath::vectAngle(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double x0=order->getParam(1)->toNumber();
        double y0=order->getParam(2)->toNumber();
        double x1=order->getParam(3)->toNumber();
        double y1=order->getParam(4)->toNumber();

        out->setValue((double)AKCommon::vectAngle(x0,y0,x1,y1));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //计算线段长度
    bool AKHandleMath::vectLength(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double x0=order->getParam(1)->toNumber();
        double y0=order->getParam(2)->toNumber();
        double x1=order->getParam(3)->toNumber();
        double y1=order->getParam(4)->toNumber();

        double len=sqrt(pow(x1-x0,2)+pow(y1-y0,2));

        out->setValue((double)len);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取最大值
    bool AKHandleMath::order_max(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double num=order->getParam(1)->toNumber();
        double rtNum=num;
        uint32 count=order->varParams.size();
        uint32 i;
        for(i=2;i<count;++i)
        {
            double num=order->getParam(i)->toNumber();
            if(num>rtNum)
            {
                rtNum=num;
            }
        }

        out->setValue((double)rtNum);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取最小值
    bool AKHandleMath::order_min(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double num=order->getParam(1)->toNumber();
        double rtNum=num;
        uint32 count=order->varParams.size();
        uint32 i;
        for(i=2;i<count;++i)
        {
            double num=order->getParam(i)->toNumber();
            if(num<rtNum)
            {
                rtNum=num;
            }
        }

        out->setValue((double)rtNum);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //绝对值
    bool AKHandleMath::order_abs(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)abs(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //数值限制
    bool AKHandleMath::order_limit(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *numVar=order->getParam(1);//数值
        AKVar *minVar=order->getParam(2);//最小值
        AKVar *maxVar=order->getParam(3);//最大值

        double num=numVar->toNumber();
        double min=minVar->toNumber();
        double max=maxVar->toNumber();
        if(num<min)num=min;
        else if(num>max)num=max;

        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //base64
    bool AKHandleMath::base64(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string &data=order->getParam(1)->toStdString();
        std::string code=AKCommon::base64(data);
        out->setValue(code);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //unbase64
    bool AKHandleMath::unbase64(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string &data=order->getParam(1)->toStdString();
        std::string code=AKCommon::unbase64(data);
        out->setValue(code);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //sha1
    bool AKHandleMath::sha1(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        if(order->getParam(1)->getType()==AKVar::typeHandle)//buf
        {
            container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
            if(!script->getCore()->checkBuf(buf))
            {
                onRunError(order,"无效buf");
                return false;//暂停执行脚本
            }

            uint32 oldPos=buf->getPos();
            container::AKUseStaticMem mem(getScript()->getCore()->mStaticMem);//临时缓存
            int8 *tmp=mem.alloc(buf->size());
            buf->setPos(0);
            buf->read(tmp,buf->size());
            std::string code=AKCommon::sha1(tmp,buf->size());
            buf->setPos(oldPos);
            out->setValue(code);
        }
        else//string
        {
            const int8 *data=order->getParam(1)->toString();
            std::string code=AKCommon::sha1(data,order->getParam(1)->toStringSize());
            out->setValue(code);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //md5
    bool AKHandleMath::md5(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        if(order->getParam(1)->getType()==AKVar::typeHandle)//buf
        {
            container::AKBuf *buf=(container::AKBuf *)order->getParam(1)->toHandle();
            if(!script->getCore()->checkBuf(buf))
            {
                onRunError(order,"无效buf");
                return false;//暂停执行脚本
            }

            std::string code=AKCommon::MD5(*buf);
            out->setValue(code);
        }
        else//string
        {
            const int8 *data=order->getParam(1)->toString();
            uint32 bytes=order->getParam(1)->toStringSize();
            std::string code=AKCommon::MD5(data,bytes);
            out->setValue(code);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //开方
    bool AKHandleMath::order_Sqrt(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)sqrt(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //N次方
    bool AKHandleMath::order_Pow(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue(pow(order->getParam(1)->toNumber(),order->getParam(2)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //三角函数atan
    bool AKHandleMath::order_Atan(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)atan(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //三角函数tan
    bool AKHandleMath::order_Tan(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)tan(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //三角函数acos
    bool AKHandleMath::order_Acos(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)acos(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //三角函数cos
    bool AKHandleMath::order_Cos(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)cos(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //三角函数asin
    bool AKHandleMath::order_Asin(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)asin(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //三角函数sin
    bool AKHandleMath::order_Sin(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)sin(order->getParam(1)->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //随机数
    bool AKHandleMath::order_Random(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)rand());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //随机数最大值
    bool AKHandleMath::order_rand_max(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)RAND_MAX);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置随机种子
    bool AKHandleMath::order_srand(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        uint32 num=order->getParam(1)->toUint();
        srand(num);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //取整（四舍五入）
    bool AKHandleMath::order_Round(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double num=order->getParam(1)->toNumber();
        int64 numInt=num;
        if((num-numInt)>=0.5)++numInt;
        out->setValue((double)numInt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //取整
    bool AKHandleMath::order_Int(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        int64 num=order->getParam(1)->toNumber();
        out->setValue((double)num);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //转换成数值
    bool AKHandleMath::order_Number(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)order->getParam(1)->toNumber());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
