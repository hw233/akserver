//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEBUF_H
#define AKSERVER_AKHANDLEBUF_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;
    namespace container
    {
        class AKBuf;
    }

    //内置buf指令类
    class AKHandleBuf:public AKHandler
    {
    public:
        AKHandleBuf();
        ~AKHandleBuf();
        AKDefineCreate(AKHandleBuf);

        virtual void init(AKScript *script);
        virtual void reset();

    public:
        AKDefineScriptFunc(bufGetScriptData);
        AKDefineScriptFunc(bufCreate);
        AKDefineScriptFunc(bufDestroy);
        AKDefineScriptFunc(bufAppend);
        AKDefineScriptFunc(bufSize);
        AKDefineScriptFunc(bufClear);
        AKDefineScriptFunc(bufOffset);
        AKDefineScriptFunc(bufSetPos);
        AKDefineScriptFunc(bufGetPos);
        AKDefineScriptFunc(bufSetEnd);
        AKDefineScriptFunc(bufFormat);
        AKDefineScriptFunc(bufWriteUint8);
        AKDefineScriptFunc(bufReadUint8);
        AKDefineScriptFunc(bufWriteInt8);
        AKDefineScriptFunc(bufReadInt8);
        AKDefineScriptFunc(bufWriteUint16);
        AKDefineScriptFunc(bufReadUint16);
        AKDefineScriptFunc(bufWriteInt16);
        AKDefineScriptFunc(bufReadInt16);
        AKDefineScriptFunc(bufWriteUint32);
        AKDefineScriptFunc(bufReadUint32);
        AKDefineScriptFunc(bufWriteInt32);
        AKDefineScriptFunc(bufReadInt32);
        AKDefineScriptFunc(bufWriteFloat);
        AKDefineScriptFunc(bufReadFloat);
        AKDefineScriptFunc(bufWriteDouble);
        AKDefineScriptFunc(bufReadDouble);
        AKDefineScriptFunc(bufWriteString);
        AKDefineScriptFunc(bufReadString);
        AKDefineScriptFunc(bufWriteHandle);
        AKDefineScriptFunc(bufReadHandle);
        AKDefineScriptFunc(bufWriteUint32String);
        AKDefineScriptFunc(bufReadUint32String);
        AKDefineScriptFunc(packStr);
        AKDefineScriptFunc(buf2hex);
        AKDefineScriptFunc(hex2buf);
        AKDefineScriptFunc(isBuf);

        AKDefineScriptFunc(serialize);
        AKDefineScriptFunc(unserialize);

        AKDefineScriptFunc(buf2str);
        AKDefineScriptFunc(str2buf);

        AKDefineScriptFunc(structMgr_create);
        AKDefineScriptFunc(structMgr_destroy);
        AKDefineScriptFunc(structMgr_load);
        AKDefineScriptFunc(structMgr_upgradeObj);
        AKDefineScriptFunc(structMgr_getNameArr);

    private:
        AKBufMap mBufMap;
        AKHandleBuf *mParent;
    };
}

#endif //AKSERVER_AKHANDLEBUF_H
