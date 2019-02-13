//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLENET_H
#define AKSERVER_AKHANDLENET_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //网络指令类
    class AKHandleNet:public AKHandler
    {
    public:
        AKHandleNet();
        ~AKHandleNet();
        AKDefineCreate(AKHandleNet);

        virtual void init(AKScript *script);

    public:
        AKDefineScriptFunc(domain2ip);

        AKDefineScriptFunc(cCreate);
        AKDefineScriptFunc(cRemove);
        AKDefineScriptFunc(cStart);
        AKDefineScriptFunc(cClose);
        AKDefineScriptFunc(cIsClose);
        AKDefineScriptFunc(cReqRecv);
        AKDefineScriptFunc(cGetRecvSize);
        AKDefineScriptFunc(cRecv);
        AKDefineScriptFunc(cPop);
        AKDefineScriptFunc(cSend);
        AKDefineScriptFunc(cIP);
        AKDefineScriptFunc(cGetSendBufSize);
        AKDefineScriptFunc(cCnn);
        AKDefineScriptFunc(cHasCnn);
        AKDefineScriptFunc(cHasLost);
        AKDefineScriptFunc(cHasRecv);
        AKDefineScriptFunc(cSetDomain);
        AKDefineScriptFunc(cSetPort);

        AKDefineScriptFunc(sCreate);
        AKDefineScriptFunc(sStart);
        AKDefineScriptFunc(sClose);
        AKDefineScriptFunc(sIsClose);
        AKDefineScriptFunc(sAccept);
        AKDefineScriptFunc(sReqRecv);
        AKDefineScriptFunc(sHasRecv);
        AKDefineScriptFunc(sGetRecvSize);
        AKDefineScriptFunc(sRecv);
        AKDefineScriptFunc(sPop);
        AKDefineScriptFunc(sGetLost);
        AKDefineScriptFunc(sCloseClient);
        AKDefineScriptFunc(sSend);
        AKDefineScriptFunc(sIP);
        AKDefineScriptFunc(sActiveClientIDArr);

        AKDefineScriptFunc(s2Create);
        AKDefineScriptFunc(s2HasConnect);
        AKDefineScriptFunc(s2HasDisconnect);
        AKDefineScriptFunc(s2HasData);
        AKDefineScriptFunc(s2GetData);
        AKDefineScriptFunc(s2Send);
        AKDefineScriptFunc(s2CloseClient);
        AKDefineScriptFunc(s2CtrlSend);
        AKDefineScriptFunc(s2SessionIP);

        AKDefineScriptFunc(webSocketShake);
        AKDefineScriptFunc(webSocketDecode);
        AKDefineScriptFunc(webSocketEncode);

        AKDefineScriptFunc(c2Create);
        AKDefineScriptFunc(c2Destroy);
        AKDefineScriptFunc(c2Connect);
        AKDefineScriptFunc(c2HasConnect);
        AKDefineScriptFunc(c2HasNetError);
        AKDefineScriptFunc(c2HasData);
        AKDefineScriptFunc(c2GetData);
        AKDefineScriptFunc(c2Send);
        AKDefineScriptFunc(c2Close);
    };
}

#endif //AKSERVER_AKHANDLENET_H
