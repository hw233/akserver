//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKVAR_H
#define AKSERVER_AKVAR_H

#include "ak.h"

namespace ak
{
    //变量类
    class AKVar
    {
    public:
        AKVar(AKCore *core);
        ~AKVar();

        //是否为引用
        bool isRef();

        //是否为引用
        bool isTmpRef();

        //引用其它变量
        void ref(
                AKVar *var,//需要引用的变量
                bool isTmpRef=false//是否为临时引用
        );

        //取消引用
        void unRef();

        //返回变量实例
        AKVar *getInst();

        //清除类型和值
        void clearType();

        //初始化值
        void initValue(
                const int8 *val,//初始值
                uint32 valType//初始类型
        );

        //重置为初始值
        void resetValue();

        //设置值
        void setValue(const int8 *val);
        void setValue(const std::string &val);
        void setValue(double val);
        void setValue(void *handle);
        void setValue(AKVar *var);

        //设置变量类型（只能设置一次，第二次设置时无效）
        void setType(uint32 type);

        //返回变量类型
        uint32 getType();

        //返回字符串
        const int8 *toString();
        std::string &toStdString();

        //返回字符串尺寸
        uint32 toStringSize();

        //返回数值
        double toNumber();

        //返回整数值
        int32 toInt();

        //返回无符号整数值
        uint32 toUint();

        //返回句柄
        void *toHandle();

        void tryRetainHandle(void *handle/*,int32 debugID*/);//尝试引用句柄
        void tryReleaseHandle(void *handle/*,int32 debugID*/);//尝试释放句柄

    public:
        static const uint32 typeString=1;//字符串类型
        static const uint32 typeNumber=2;//数值类型
        static const uint32 typeHandle=3;//句柄类型
        static const uint32 typeRef=4;//引用类型

        //AK_DEBUG//不用时删除掉
        //std::string mName;//变量名

    private:
        AKCore *mCore;

        uint32 type;//类型
        std::string strVal;//字符串值
        double numVal;//数值
        void *handleVal;//句柄
        std::string initVal;//初始值
        uint32 initValType;//初始类型

        //引用相关
        AKVar *mRefVar;//引用的变量
        AKVarVarMap mRefParent;//引用当前变量的所有父
        bool mIsTmpRef;
    };
}

#endif //AKSERVER_AKVAR_H
