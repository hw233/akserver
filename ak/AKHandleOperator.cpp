#include "ak.h"
#include "AKScript.h"
#include "AKHandleBuf.h"
#include "AKHandleProcess.h"
#include "AKHandleOperator.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleOperator::AKHandleOperator()
            :mHandleBuf(NULL),
             mHandleProcess(NULL)
    {
    }

    AKHandleOperator::~AKHandleOperator()
    {
    }

    AKHandleBuf *AKHandleOperator::getHandleBuf()
    {
        if(!mHandleBuf)
        {
            mHandleBuf=(AKHandleBuf *)getScript()->getHandler("AKHandleBuf");
        }
        return mHandleBuf;
    }

    AKHandleProcess *AKHandleOperator::getHandleProcess()
    {
        if(!mHandleProcess)
        {
            mHandleProcess=(AKHandleProcess *)getScript()->getHandler("AKHandleProcess");
        }
        return mHandleProcess;
    }

    void AKHandleOperator::init(AKScript *script)
    {
        AKHandler::init(script);

        AKQuickBindScriptFunc("try",&AKHandleOperator::order_Try,"try();");//启动异常处理机制
        AKQuickBindScriptFunc("throw",&AKHandleOperator::order_throw,"throw(\"error msg\");//抛出异常");
        AKQuickBindScriptFunc("catch",&AKHandleOperator::order_Catch,"catch();");//捕获异常后执行的指令
        AKQuickBindScriptFunc("error",&AKHandleOperator::order_Error,"errorString=error();");//获取异常信息

        AKQuickBindScriptFunc("goto",&AKHandleOperator::order_Goto,"");//跳转
        AKQuickBindScriptFunc("if_goto",&AKHandleOperator::order_IfGoto,"");//条件跳转
        AKQuickBindScriptFunc("end",&AKHandleOperator::order_End,"end();//结束执行脚本");
        AKQuickBindScriptFunc("exit",&AKHandleOperator::order_End,"exit();//结束执行脚本");
        AKQuickBindScriptFunc("type",&AKHandleOperator::order_type,"str=type(varName);//返回变更类型");
        AKQuickBindScriptFunc("reset",&AKHandleOperator::order_reset,"reset(var);//重置变量为未初始化");

        AKQuickBindScriptFunc("=",&AKHandleOperator::order_Equal,"");//赋值=
        AKQuickBindScriptFunc("+=",&AKHandleOperator::order_Equal2,"");//赋值+=
        AKQuickBindScriptFunc("-=",&AKHandleOperator::order_Equal3,"");//赋值-=
        AKQuickBindScriptFunc("*=",&AKHandleOperator::order_Equal4,"");//赋值*=
        AKQuickBindScriptFunc("/=",&AKHandleOperator::order_Equal5,"");//赋值/=
        AKQuickBindScriptFunc("%=",&AKHandleOperator::order_Equal11,"");//赋值/=
        AKQuickBindScriptFunc("&=",&AKHandleOperator::order_Equal6,"");//赋值&=
        AKQuickBindScriptFunc("^=",&AKHandleOperator::order_Equal7,"");//赋值^=
        AKQuickBindScriptFunc("|=",&AKHandleOperator::order_Equal8,"");//赋值|=
        AKQuickBindScriptFunc("<<=",&AKHandleOperator::order_Equal9,"");//赋值<<=
        AKQuickBindScriptFunc(">>=",&AKHandleOperator::order_Equal10,"");//赋值>>=

        AKQuickBindScriptFunc("++",&AKHandleOperator::order_one1,"");//单目++
        AKQuickBindScriptFunc("--",&AKHandleOperator::order_one2,"");//单目--
        AKQuickBindScriptFunc("!",&AKHandleOperator::order_one3,"");//单目!
        AKQuickBindScriptFunc("~",&AKHandleOperator::order_one4,"");//单目~
        AKQuickBindScriptFunc(" +",&AKHandleOperator::order_one5,"");//单目正号
        AKQuickBindScriptFunc(" -",&AKHandleOperator::order_one6,"");//单目负号

        AKQuickBindScriptFunc("+",&AKHandleOperator::order_Add,"");//双目+
        AKQuickBindScriptFunc("-",&AKHandleOperator::order_Sub,"");//双目-
        AKQuickBindScriptFunc("*",&AKHandleOperator::order_Mul,"");//双目*
        AKQuickBindScriptFunc("/",&AKHandleOperator::order_Div,"");//双目/
        AKQuickBindScriptFunc("%",&AKHandleOperator::order_two5,"");//双目%
        AKQuickBindScriptFunc("<<",&AKHandleOperator::order_two6,"");//双目<<
        AKQuickBindScriptFunc(">>",&AKHandleOperator::order_two7,"");//双目>>
        AKQuickBindScriptFunc("<",&AKHandleOperator::order_two8,"");//双目<
        AKQuickBindScriptFunc("<=",&AKHandleOperator::order_two9,"");//双目<=
        AKQuickBindScriptFunc(">",&AKHandleOperator::order_two10,"");//双目>
        AKQuickBindScriptFunc(">=",&AKHandleOperator::order_two11,"");//双目>=
        AKQuickBindScriptFunc("==",&AKHandleOperator::order_two12,"");//双目==
        AKQuickBindScriptFunc("!=",&AKHandleOperator::order_two13,"");//双目!=
        AKQuickBindScriptFunc("&",&AKHandleOperator::order_two14,"");//双目&
        AKQuickBindScriptFunc("^",&AKHandleOperator::order_two15,"");//双目^
        AKQuickBindScriptFunc("|",&AKHandleOperator::order_two16,"");//双目|
        AKQuickBindScriptFunc("&&",&AKHandleOperator::order_two17,"");//双目&&
        AKQuickBindScriptFunc("||",&AKHandleOperator::order_two18,"");//双目||

        //成员运算符
        AKQuickBindScriptFunc(".",&AKHandleOperator::getMember,"t=a.b;");//点成员运算符
        AKQuickBindScriptFunc("[]",&AKHandleOperator::getMember,"t=a[b];");//方括号成员运算符
    }

    //成员运算符
    bool AKHandleOperator::getMember(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        int8 error[1024];
        AKVar *var=AKCommon::getVarFromContainer(script,order,error,true);
        if(!var)
        {
            onRunError(order,error);
            return false;//暂停执行脚本
        }

        out->ref(var);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //启动异常处理机制
    bool AKHandleOperator::order_Try(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        //启动异常处理机制//一旦捕获异常，将向下找到最近的catch指令位置开始执行
        script->startTry();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //抛出异常
    bool AKHandleOperator::order_throw(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        onRunError(order,order->getParam(1)->toString());
        return false;//暂停执行脚本

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    bool AKHandleOperator::order_Catch(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        //关闭异常处理机制
        script->closeTry();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    bool AKHandleOperator::order_Error(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        out->setValue((const int8 *)script->catchError());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //重置变量为未初始化
    bool AKHandleOperator::order_reset(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKVar *in1=order->getParam(1);
        in1->clearType();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //返回变量类型
    bool AKHandleOperator::order_type(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKVar *in1=order->getParam(1);
        std::string val;
        if(in1->getType()==AKVar::typeString)val="string";
        else if(in1->getType()==AKVar::typeNumber)val="number";
        else if(in1->getType()==AKVar::typeHandle)
        {
            AKRef *ref=(AKRef *)in1->toHandle();
            if(script->getCore()->checkDict(ref))val="dict";
            else if(script->getCore()->checkArr(ref))val="arr";
            else if(script->getCore()->checkBuf(ref))val="buf";
            else val="handle";
        }
        else val="null";
        out->setValue(val.c_str());

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //跳转
    bool AKHandleOperator::order_Goto(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *orderPos=order->getParam(1);//指令编号
        out->setValue((double)1);
        script->setOrderPos(orderPos->toNumber());//跳转到指定的指令
        return true;//继续执行脚本
    }

    //条件跳转
    bool AKHandleOperator::order_IfGoto(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *expValue=order->getParam(1);//条件表达式结果
        AKVar *orderPos=order->getParam(2);//指令编号
        if(expValue->toUint()!=0)//条件成立
        {
            out->setValue((double)1);
            script->setOrderPos(orderPos->toNumber());//跳转到指定的指令
        }
        else//条件不成立
        {
            out->setValue((double)0);
            script->nextOrder();//下一行指令
        }

        return true;//继续执行脚本
    }

    //结束执行脚本
    bool AKHandleOperator::order_End(AKScript *script,AKOrder *order)
    {
        script->setOrderPos(script->getOrderCount());
        return true;//继续执行脚本
    }

    //双目||
    bool AKHandleOperator::order_two18(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->toUint()||in2->toUint())out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目&&
    bool AKHandleOperator::order_two17(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->toUint()&&in2->toUint())out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目|
    bool AKHandleOperator::order_two16(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        out->setValue((double)(in1->toUint()|in2->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目^
    bool AKHandleOperator::order_two15(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        out->setValue((double)(in1->toUint()^in2->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目&
    bool AKHandleOperator::order_two14(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        out->setValue((double)(in1->toUint()&in2->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目!=
    bool AKHandleOperator::order_two13(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->getType()==AKVar::typeNumber&&in2->getType()==AKVar::typeNumber)
        {
            if(in1->toNumber()!=in2->toNumber())out->setValue((double)1);
            else out->setValue((double)0);
        }
        else
        {
            if(strcmp(in1->toString(),in2->toString())!=0)out->setValue((double)1);
            else out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目==
    bool AKHandleOperator::order_two12(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->getType()==AKVar::typeNumber&&in2->getType()==AKVar::typeNumber)
        {
            if(in1->toNumber()==in2->toNumber())out->setValue((double)1);
            else out->setValue((double)0);
        }
        else
        {
            if(strcmp(in1->toString(),in2->toString())==0)out->setValue((double)1);
            else out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目>=
    bool AKHandleOperator::order_two11(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->toNumber()>=in2->toNumber())out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目>
    bool AKHandleOperator::order_two10(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->toNumber()>in2->toNumber())out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目<=
    bool AKHandleOperator::order_two9(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->toNumber()<=in2->toNumber())out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目<
    bool AKHandleOperator::order_two8(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        if(in1->toNumber()<in2->toNumber())out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目>>
    bool AKHandleOperator::order_two7(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        out->setValue((double)(in1->toUint()>>in2->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目<<
    bool AKHandleOperator::order_two6(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);

        out->setType(AKVar::typeNumber);//设置返回类型
        out->setValue((double)(in1->toUint()<<in2->toUint()));

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //双目%
    bool AKHandleOperator::order_two5(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        int32 in1=order->getParam(1)->toInt();
        int32 in2=order->getParam(2)->toInt();

        if(in2==0)
        {
            onRunError(order,"% 除0出错");
            return false;//暂停执行脚本
        }

        out->setValue((double)(in1%in2));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //单目负号
    bool AKHandleOperator::order_one6(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(-in1->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //单目正号
    bool AKHandleOperator::order_one5(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)in1->toNumber());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //单目~
    bool AKHandleOperator::order_one4(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(~in1->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //单目!
    bool AKHandleOperator::order_one3(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        if(in1->toNumber()==0)out->setValue((double)1);
        else out->setValue((double)0);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //单目--
    bool AKHandleOperator::order_one2(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(in1->toNumber()-1));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //单目++
    bool AKHandleOperator::order_one1(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(in1->toNumber()+1));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值>>=
    bool AKHandleOperator::order_Equal10(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toUint()>>in1->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值<<=
    bool AKHandleOperator::order_Equal9(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toUint()<<in1->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值|=
    bool AKHandleOperator::order_Equal8(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toUint()|in1->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值^=
    bool AKHandleOperator::order_Equal7(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toUint()^in1->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值&=
    bool AKHandleOperator::order_Equal6(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toUint()&in1->toUint()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值/=
    bool AKHandleOperator::order_Equal5(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        double numIn1=in1->toNumber();
        if(numIn1==0)
        {
            onRunError(order,"/= 除0出错");
            return false;//暂停执行脚本
        }
        out->setValue((double)(out->toNumber()/in1->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值%=
    bool AKHandleOperator::order_Equal11(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        double numIn1=in1->toNumber();
        if(numIn1==0)
        {
            onRunError(order,"%= 除0出错");
            return false;//暂停执行脚本
        }
        out->setValue((double)(((int64)out->toNumber())%((int64)in1->toNumber())));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值*=
    bool AKHandleOperator::order_Equal4(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toNumber()*in1->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值-=
    bool AKHandleOperator::order_Equal3(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        out->setValue((double)(out->toNumber()-in1->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值+=
    bool AKHandleOperator::order_Equal2(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        AKVar *in1=order->getParam(1);

        if(out->getType()==0)//输出变量类型未确定
        {
            //设置返回类型
            if(in1->getType()==AKVar::typeNumber)
            {
                out->setType(AKVar::typeNumber);
                out->setValue((double)in1->toNumber());
            }
            else if(in1->getType()==AKVar::typeString)
            {
                out->setType(AKVar::typeString);
                out->setValue(in1->toStdString());
            }
        }
        else//输出变量类型已确定
        {
            if(out->getType()==AKVar::typeNumber&&in1->getType()==AKVar::typeNumber)//均为数值
            {
                out->setValue((double)(out->toNumber()+in1->toNumber()));
            }
            else//其中一方为字符串
            {
                std::string tmp;
                tmp+=out->toStdString();
                tmp+=in1->toStdString();
                out->setValue(tmp);
            }
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //赋值=
    bool AKHandleOperator::order_Equal(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        AKVar *in1=order->getParam(1);

        if(in1->isTmpRef())//输入为临时引用
        {
            //设置新引用
            out->ref(in1);
        }
        else//输入为非临时引用
        {
            //设置返回类型
            if(in1->getType()==AKVar::typeNumber)
            {
                out->setType(AKVar::typeNumber);
                out->setValue(in1->toNumber());
            }
            else if(in1->getType()==AKVar::typeString)
            {
                out->setType(AKVar::typeString);
                out->setValue(in1->toStdString());
            }
            else if(in1->getType()==AKVar::typeHandle)
            {
                out->setType(AKVar::typeHandle);
                out->setValue(in1->toHandle());
            }
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //加法
    bool AKHandleOperator::order_Add(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);

        if(in1->getType()==AKVar::typeNumber&&in2->getType()==AKVar::typeNumber)//均为数值
        {
            out->setType(AKVar::typeNumber);//设置返回类型
            out->setValue((double)(in1->toNumber()+in2->toNumber()));
        }
        else//其中一方为字符串
        {
            out->setType(AKVar::typeString);//设置返回类型
            std::string tmp;
            tmp+=in1->toStdString();
            tmp+=in2->toStdString();
            out->setValue(tmp);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //减法
    bool AKHandleOperator::order_Sub(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        out->setValue((double)(in1->toNumber()-in2->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //乘法
    bool AKHandleOperator::order_Mul(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        out->setValue((double)(in1->toNumber()*in2->toNumber()));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //除法
    bool AKHandleOperator::order_Div(AKScript *script,AKOrder *order)
    {
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *in1=order->getParam(1);
        AKVar *in2=order->getParam(2);
        double numIn2=in2->toNumber();
        if(numIn2==0)
        {
            onRunError(order,"/ 除0出错");
            return false;//暂停执行脚本
        }
        out->setValue((double)(in1->toNumber()/numIn2));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
