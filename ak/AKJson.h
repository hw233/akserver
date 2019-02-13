//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKJSON_H
#define AKSERVER_AKJSON_H

namespace ak
{
    struct AKJsonWord
    {
        int type;//类型：0=运算符;1=数值;2=字符串;
        std::string val;//值
    };

    //JSON类
    class AKJson
    {
    public:
        static bool decode(AKCore *core,AKVar *var, const int8 *str, int32 strSize);
    };
}

#endif //AKSERVER_AKJSON_H
