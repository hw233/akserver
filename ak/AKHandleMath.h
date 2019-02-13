//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEMATH_H
#define AKSERVER_AKHANDLEMATH_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;
    class AKHandleBuf;

    //内置数学指令类
    class AKHandleMath:public AKHandler
    {
    public:
        AKHandleMath();
        ~AKHandleMath();
        AKDefineCreate(AKHandleMath);

        virtual void init(AKScript *script);

    public:
        AKDefineScriptFunc(log10);
        AKDefineScriptFunc(uuid);
        AKDefineScriptFunc(crc32);
        AKDefineScriptFunc(base64);
        AKDefineScriptFunc(unbase64);
        AKDefineScriptFunc(urlencode);
        AKDefineScriptFunc(urldecode);
        AKDefineScriptFunc(sha1);
        AKDefineScriptFunc(order_Random);
        AKDefineScriptFunc(order_srand);
        AKDefineScriptFunc(order_rand_max);
        AKDefineScriptFunc(order_Round);
        AKDefineScriptFunc(order_Int);
        AKDefineScriptFunc(order_Number);
        AKDefineScriptFunc(order_Sin);
        AKDefineScriptFunc(order_Asin);
        AKDefineScriptFunc(order_Cos);
        AKDefineScriptFunc(order_Acos);
        AKDefineScriptFunc(order_Tan);
        AKDefineScriptFunc(order_Atan);
        AKDefineScriptFunc(order_Pow);
        AKDefineScriptFunc(order_Sqrt);
        AKDefineScriptFunc(md5);
        AKDefineScriptFunc(order_max);
        AKDefineScriptFunc(order_min);
        AKDefineScriptFunc(order_limit);
        AKDefineScriptFunc(order_abs);
        AKDefineScriptFunc(order_ceil);
        AKDefineScriptFunc(order_floor);

        AKDefineScriptFunc(moveTo);
        AKDefineScriptFunc(vectLength);
        AKDefineScriptFunc(vectAngle);
        AKDefineScriptFunc(angle2radian);
        AKDefineScriptFunc(radian2angle);

    private:
        AKHandleBuf *getHandleBuf();

    private:
        AKHandleBuf *mHandleBuf;
    };
}


#endif //AKSERVER_AKHANDLEMATH_H
