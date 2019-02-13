//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKORDER_H
#define AKSERVER_AKORDER_H

namespace ak
{
    //指令类
    class AKOrder
    {
    public:
        AKOrder();
        ~AKOrder();

        //获取参数
        AKVar *getParam(
                uint32 index//0:out;>=1:参数1...
        );

    public:
        AKStringArray params;//指令信息数据[orderName,out,var1,var2...]
        AKScriptFunc func;//函数对象
        AKVarArray varParams;//实际参数对象数组[out,var1,var2...]

        AKVar *scriptFuncVar;//脚本句柄变量或脚本url字符串变量
        std::string apiName;//注册的API名（对应脚本函数）
        std::string rowNum;//行号
    };
}

#endif //AKSERVER_AKORDER_H
