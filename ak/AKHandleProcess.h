//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEPROCESS_H
#define AKSERVER_AKHANDLEPROCESS_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //内置进程指令类
    class AKHandleProcess:public AKHandler
    {
    public:
        AKHandleProcess();
        ~AKHandleProcess();
        AKDefineCreate(AKHandleProcess);

        virtual void init(AKScript *script);
        virtual void reset();

        AKHandleBuf *getHandleBuf();

    public:
        AKDefineScriptFunc(pSet);
        AKDefineScriptFunc(pGet);
        AKDefineScriptFunc(pGetRef);
        AKDefineScriptFunc(pRemove);
        AKDefineScriptFunc(pExist);

        AKDefineScriptFunc(coreID);
        AKDefineScriptFunc(coreCount);
        AKDefineScriptFunc(sysCoreID);

        AKDefineScriptFunc(_handle);
        AKDefineScriptFunc(empty);
        AKDefineScriptFunc(pCreate);
        AKDefineScriptFunc(pCreateNoBreakPoint);
        AKDefineScriptFunc(pScriptUrl);
        AKDefineScriptFunc(pID);
        AKDefineScriptFunc(pKill);
        AKDefineScriptFunc(pBind);

        AKDefineScriptFunc(pSend);
        AKDefineScriptFunc(pRecv);

        AKDefineScriptFunc(pRequest);
        AKDefineScriptFunc(pHasRequest);
        AKDefineScriptFunc(pResponse);

        AKDefineScriptFunc(func);
        AKDefineScriptFunc(exec2);
        AKDefineScriptFunc(input);
        AKDefineScriptFunc(inputCount);
        AKDefineScriptFunc(_return);

        AKDefineScriptFunc(pDt);

        AKDefineScriptFunc(mqCreate);
        AKDefineScriptFunc(mqRemove);
        AKDefineScriptFunc(mqSend);
        AKDefineScriptFunc(mqRecv);

    public:
        AKScript *mScriptFunc;//正在调用的子级脚本函数

    private:
        AKProcess *mProcess;
        AKHandleBuf *mHandleBuf;

        AKHandleProcess *mParent;
    };
}

#endif //AKSERVER_AKHANDLEPROCESS_H
