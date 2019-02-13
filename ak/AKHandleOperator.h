//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEOPERATOR_H
#define AKSERVER_AKHANDLEOPERATOR_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;
    class AKHandleBuf;
    class AKHandleProcess;

    //内置运算指令类
    class AKHandleOperator:public AKHandler
    {
    public:
        AKHandleOperator();
        ~AKHandleOperator();
        AKDefineCreate(AKHandleOperator);

        virtual void init(AKScript *script);

    public:
        AKDefineScriptFunc(order_Try);//启动异常处理机制
        AKDefineScriptFunc(order_throw);//抛出异常
        AKDefineScriptFunc(order_Catch);//捕获异常后执行的指令
        AKDefineScriptFunc(order_Error);//获取异常信息

        AKDefineScriptFunc(order_Goto);//跳转
        AKDefineScriptFunc(order_IfGoto);//条件跳转
        AKDefineScriptFunc(order_End);
        AKDefineScriptFunc(order_type);
        AKDefineScriptFunc(order_reset);

        AKDefineScriptFunc(order_Equal);//赋值=
        AKDefineScriptFunc(order_Equal2);//赋值+=
        AKDefineScriptFunc(order_Equal3);//赋值-=
        AKDefineScriptFunc(order_Equal4);//赋值*=
        AKDefineScriptFunc(order_Equal5);//赋值/=
        AKDefineScriptFunc(order_Equal11);//赋值%=
        AKDefineScriptFunc(order_Equal6);//赋值&=
        AKDefineScriptFunc(order_Equal7);//赋值^=
        AKDefineScriptFunc(order_Equal8);//赋值|=
        AKDefineScriptFunc(order_Equal9);//赋值<<=
        AKDefineScriptFunc(order_Equal10);//赋值>>=

        AKDefineScriptFunc(order_one1);//单目++
        AKDefineScriptFunc(order_one2);//单目--
        AKDefineScriptFunc(order_one3);//单目!
        AKDefineScriptFunc(order_one4);//单目~
        AKDefineScriptFunc(order_one5);//单目正号
        AKDefineScriptFunc(order_one6);//单目负号

        AKDefineScriptFunc(order_Add);//双目+
        AKDefineScriptFunc(order_Sub);//双目-
        AKDefineScriptFunc(order_Mul);//双目*
        AKDefineScriptFunc(order_Div);//双目/
        AKDefineScriptFunc(order_two5);//双目%
        AKDefineScriptFunc(order_two6);//双目<<
        AKDefineScriptFunc(order_two7);//双目>>
        AKDefineScriptFunc(order_two8);//双目<
        AKDefineScriptFunc(order_two9);//双目<=
        AKDefineScriptFunc(order_two10);//双目>
        AKDefineScriptFunc(order_two11);//双目>=
        AKDefineScriptFunc(order_two12);//双目==
        AKDefineScriptFunc(order_two13);//双目!=
        AKDefineScriptFunc(order_two14);//双目&
        AKDefineScriptFunc(order_two15);//双目^
        AKDefineScriptFunc(order_two16);//双目|
        AKDefineScriptFunc(order_two17);//双目&&
        AKDefineScriptFunc(order_two18);//双目||
        AKDefineScriptFunc(getMember);//成员运算符

    private:
        AKHandleBuf *getHandleBuf();
        AKHandleProcess *getHandleProcess();

    private:
        AKHandleBuf *mHandleBuf;
        AKHandleProcess *mHandleProcess;
    };
}

#endif //AKSERVER_AKHANDLEOPERATOR_H
