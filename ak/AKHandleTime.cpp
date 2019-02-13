#include "ak.h"
#include "AKScript.h"
#include "AKHandleTime.h"
#include "AKGlobal.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleTime::AKHandleTime()
            :mT0(0),
             mT_Delay(0)
    {
    }

    AKHandleTime::~AKHandleTime()
    {
    }

    void AKHandleTime::init(AKScript *script)
    {
        AKHandler::init(script);

        AKQuickBindScriptFunc("delay",&AKHandleTime::_delay,"delay(1000);//延时（毫秒）");
        AKQuickBindScriptFunc("sleep",&AKHandleTime::_delay,"sleep(1000);//延时（毫秒）");

        AKQuickBindScriptFunc("dt",&AKHandleTime::dt,"dt=dt();//当前脚本中，返回与上次dt调用的间隔时间（毫秒）");
        AKQuickBindScriptFunc("time",&AKHandleTime::time,"t1=time();//获取整数时间戳（秒）");
        AKQuickBindScriptFunc("time_milli",&AKHandleTime::time_milli,"t1=time_milli();//获取整数时间戳（毫秒）");
        AKQuickBindScriptFunc("time_micro",&AKHandleTime::time_micro,"t1=time_micro();//获取整数时间戳（微秒）");
        AKQuickBindScriptFunc("tick",&AKHandleTime::tick,"t1=tick();//获取整数时间戳（毫秒）");
        AKQuickBindScriptFunc("isTimeout",&AKHandleTime::isTimeout,"isTimeout=isTimeout(time,dt);//判断是否超时（毫秒）");
        AKQuickBindScriptFunc("str2time",&AKHandleTime::str2time,"t1=str2time(\"2013-10-14 10:07:30\");//日期字符串转整数时间戳（秒）");
        AKQuickBindScriptFunc("strtotime",&AKHandleTime::str2time,"t1=strtotime(\"2013-10-14 10:07:30\");t1=strtotime(\"2013-10-14\");//日期字符串转整数时间戳（秒）");
        AKQuickBindScriptFunc("time2str",&AKHandleTime::timetostr,"str=time2str(t1);//时间戳（从1970年开始计算的秒数）转日期时间字符串");
        AKQuickBindScriptFunc("timetostr",&AKHandleTime::timetostr,"str=timetostr(t1);//时间戳（从1970年开始计算的秒数）转日期时间字符串");
        AKQuickBindScriptFunc("timeInfo",&AKHandleTime::timeInfo,"timeInfo(yy=0,mm=0,dd=0,h=0,m=0,s=0);//获取日期时间");
        AKQuickBindScriptFunc("now",&AKHandleTime::now,"datetimeStr=now();//获取日期时间字符串");
    }

    //获取日期时间字符串
    bool AKHandleTime::now(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string datetimeStr=AKCommon::now();

        out->setValue(datetimeStr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //当前脚本中，返回与上次dt调用的间隔时间（毫秒）
    bool AKHandleTime::dt(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double t1= AKCommon::time_milli();
        double dt=t1-mT0;
        mT0=t1;

        out->setValue((double)dt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取日期时间
    bool AKHandleTime::timeInfo(AKScript *script,AKOrder *order)
    {
        if(!checkParams(6,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        uint32 yy,mm,dd,h,m,s;
        AKCommon::timeInfo(yy,mm,dd,h,m,s);

        order->getParam(1)->setValue((double)yy);
        order->getParam(2)->setValue((double)mm);
        order->getParam(3)->setValue((double)dd);
        order->getParam(4)->setValue((double)h);
        order->getParam(5)->setValue((double)m);
        order->getParam(6)->setValue((double)s);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断是否超时
    bool AKHandleTime::isTimeout(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *timeVar=order->getParam(1);//计时变量
        double dt=order->getParam(2)->toNumber();//时间间隔（毫秒）

        double curTime=AKCommon::time_milli();
        double curDt=curTime-timeVar->toNumber();//已过时间
        if(curDt>=dt)//超时
        {
            timeVar->setValue(curTime);//重置
            out->setValue((double)1);
        }
        else//未超时
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取tick毫秒数
    bool AKHandleTime::tick(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)AKCommon::time_milli());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //时间（从1970年开始计算的秒数）转日期时间字符串
    bool AKHandleTime::timetostr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        double time=order->getParam(1)->toNumber();

        out->setValue(AKCommon::timetostr((int64)time).c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //日期字符串转整数时间戳（秒）
    bool AKHandleTime::str2time(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *strDatetime=order->getParam(1)->toString();

        double time=-1;

        try
        {
            boost::posix_time::ptime t1(boost::posix_time::time_from_string("1970-01-01 00:00:00"));
            int32 t;
            int32 rt=sscanf(strDatetime,"%d-%d-%d %d:%d:%d",&t,&t,&t,&t,&t,&t);
            if(rt==6)
            {
                boost::posix_time::ptime t2(boost::posix_time::time_from_string(strDatetime));
                boost::posix_time::time_duration dt=t2-t1;
                time=dt.total_seconds();
            }
            else if(rt==3)
            {
                std::string lastDatetime=strDatetime;
                lastDatetime+=" 00:00:00";
                boost::posix_time::ptime t2(boost::posix_time::time_from_string(lastDatetime));
                boost::posix_time::time_duration dt=t2-t1;
                time=dt.total_seconds();
            }
        }
        catch(...)
        {
        }

        if(time==-1)
        {
            onRunError(order,"时间字符串格式错误，正确如：2016-5-30或2016-5-30 00:00:00");
            return false;//暂停执行脚本
        }

        //减去8小时
        time-=8*60*60;

        out->setValue((double)time);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取整数时间戳（秒）
    bool AKHandleTime::time(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double time=AKCommon::time();
        out->setValue((double)time);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取整数时间戳（毫秒）
    bool AKHandleTime::time_milli(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double time=AKCommon::time_milli();
        out->setValue((double)time);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取整数时间戳（微秒）
    bool AKHandleTime::time_micro(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double time=AKCommon::time_micro();
        out->setValue((double)time);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //延时（毫秒）
    bool AKHandleTime::_delay(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order,true))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        switch(script->getOrderStatus())
        {
            case 0:
                //mTimer.restart();
                mT_Delay= AKCommon::time_milli();
                script->setOrderStatus(1);
                break;
            default:
                //if((mTimer.elapsed()*1000)>=order->getParam(1)->toUint())
                if((AKCommon::time_milli()-mT_Delay)>=order->getParam(1)->toUint())
                {
                    out->setValue((double)1);
                    script->nextOrder();//下一行指令
                    return true;//继续执行脚本
                }
                break;
        }
        return false;//暂停执行脚本
    }

}
