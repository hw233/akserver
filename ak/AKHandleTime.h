//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLETIME_H
#define AKSERVER_AKHANDLETIME_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //内置公用指令类
    class AKHandleTime:public AKHandler
    {
    public:
        AKHandleTime();
        ~AKHandleTime();
        AKDefineCreate(AKHandleTime);

        virtual void init(AKScript *script);

    public:
        AKDefineScriptFunc(_delay);

        AKDefineScriptFunc(dt);
        AKDefineScriptFunc(tick);
        AKDefineScriptFunc(time);
        AKDefineScriptFunc(time_milli);
        AKDefineScriptFunc(time_micro);
        AKDefineScriptFunc(isTimeout);
        AKDefineScriptFunc(str2time);
        AKDefineScriptFunc(timeInfo);
        AKDefineScriptFunc(now);
        AKDefineScriptFunc(timetostr);

    private:
        //boost::timer mTimer;
        double mT0;//时间戳
        double mT_Delay;//时间戳
    };
}

#endif //AKSERVER_AKHANDLETIME_H
