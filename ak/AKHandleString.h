//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLESTRING_H
#define AKSERVER_AKHANDLESTRING_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //字符串指令类
    class AKHandleString:public AKHandler
    {
    public:
        AKHandleString();
        ~AKHandleString();
        AKDefineCreate(AKHandleString);

        virtual void init(AKScript *script);

    public:
        AKDefineScriptFunc(toupper);
        AKDefineScriptFunc(tolower);
        AKDefineScriptFunc(ord);
        AKDefineScriptFunc(chr);
        AKDefineScriptFunc(_strsub);
        AKDefineScriptFunc(_strlen);
        AKDefineScriptFunc(_string);
        AKDefineScriptFunc(strFind);
        AKDefineScriptFunc(strReplace);
        AKDefineScriptFunc(strRepeat);
        AKDefineScriptFunc(str_c_escape);
        AKDefineScriptFunc(str_c_unescape);
        AKDefineScriptFunc(str_sprintf);
        AKDefineScriptFunc(_strtol);
    };
}

#endif //AKSERVER_AKHANDLESTRING_H
