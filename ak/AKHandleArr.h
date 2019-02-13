//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEARR_H
#define AKSERVER_AKHANDLEARR_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //内置数组指令类
    class AKHandleArr:public AKHandler
    {
    public:
        AKHandleArr();
        ~AKHandleArr();
        AKDefineCreate(AKHandleArr);

        virtual void init(AKScript *script);
        virtual void reset();

    public:
        AKDefineScriptFunc(arrDestroy);
        AKDefineScriptFunc(arrCreate);
        AKDefineScriptFunc(arrSize);
        AKDefineScriptFunc(arrClear);
        AKDefineScriptFunc(arrPush);
        AKDefineScriptFunc(arrSet);
        AKDefineScriptFunc(arrGet);
        AKDefineScriptFunc(arrGetRef);
        AKDefineScriptFunc(strSplit);
        AKDefineScriptFunc(strJoin);
        AKDefineScriptFunc(arrSub);
        AKDefineScriptFunc(arrRemove);
        AKDefineScriptFunc(arrFindNumberKey);
        AKDefineScriptFunc(arrFindStringKey);
        AKDefineScriptFunc(isArr);

    private:
        AKHandleArr *mParent;
    };
}

#endif //AKSERVER_AKHANDLEARR_H
