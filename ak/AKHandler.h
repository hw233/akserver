//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLER_H
#define AKSERVER_AKHANDLER_H

namespace ak
{
    class AKScript;

    //内置指令基类
    class AKHandler
    {
    public:
        AKHandler();
        virtual ~AKHandler();
        virtual void init(AKScript *script);
        virtual void reset();

        AKScript *getScript();
        AKHandler *getTopParent();

    protected:

        //检查参数数量
        bool checkParams(
                uint32 needParamCount,//需要的最小参数数量
                AKScript *script,
                AKOrder *order,
                bool isDelayOrder=false//是否延时指令（无用项，此项为了与AKGameFrame的结构兼容）
        );

        //错误输出
        void onRunError(AKOrder *order,const int8 *error);

    protected:
        AKScript *mScript;

    public:
        std::string mName;//处理器名称
    };
}

#endif //AKSERVER_AKHANDLER_H
