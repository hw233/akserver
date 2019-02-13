//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLECOMMON_H
#define AKSERVER_AKHANDLECOMMON_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //内置公用指令类
    class AKHandleCommon:public AKHandler
    {
    public:
        AKHandleCommon();
        ~AKHandleCommon();
        AKDefineCreate(AKHandleCommon);

        virtual void init(AKScript *script);

    public:
        AKDefineScriptFunc(null);
        AKDefineScriptFunc(myargc);
        AKDefineScriptFunc(myargv);
        AKDefineScriptFunc(help);
        AKDefineScriptFunc(trace);
        AKDefineScriptFunc(echo);
        AKDefineScriptFunc(link);
        AKDefineScriptFunc(_ref);
        AKDefineScriptFunc(refCount);
        AKDefineScriptFunc(hotUpdate);
        AKDefineScriptFunc(scriptUrl);
        AKDefineScriptFunc(scriptDir);
        AKDefineScriptFunc(full);
        AKDefineScriptFunc(quitSystem);
        AKDefineScriptFunc(autoSleep);
        AKDefineScriptFunc(api);
        AKDefineScriptFunc(platform);
        AKDefineScriptFunc(fullRun);
        AKDefineScriptFunc(errorLog);
        AKDefineScriptFunc(getCoreRunTimeInfo);
        AKDefineScriptFunc(var);
        AKDefineScriptFunc(varNameArr);
        AKDefineScriptFunc(getScriptMidCode);
        AKDefineScriptFunc(_shell);

        AKDefineScriptFunc(addBreakPoint);
        AKDefineScriptFunc(delBreakPoint);
        AKDefineScriptFunc(delAllBreakPoint);
        AKDefineScriptFunc(isBreakPointExist);
        AKDefineScriptFunc(pNoBreakPoint);
        AKDefineScriptFunc(getBreakPointDict);

        AKDefineScriptFunc(gSet);
        AKDefineScriptFunc(gGet);
        AKDefineScriptFunc(gRemove);
        AKDefineScriptFunc(gExist);

        AKDefineScriptFunc(clean);
        AKDefineScriptFunc(exist);
        AKDefineScriptFunc(push);
        AKDefineScriptFunc(set);
        AKDefineScriptFunc(get);
        AKDefineScriptFunc(getRef);
        AKDefineScriptFunc(size);
        AKDefineScriptFunc(clone);
        AKDefineScriptFunc(remove);
        AKDefineScriptFunc(inArr);

    };
}

#endif //AKSERVER_AKHANDLECOMMON_H
