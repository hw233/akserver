//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSCRIPT_H
#define AKSERVER_AKSCRIPT_H

#include "ak.h"

namespace ak
{
    //脚本类
    class AKScript
    {
    public:
        AKScript(
                AKSystem *system,//系统对象
                AKCore *core,//核对象
                AKProcess *process,//进程对象
                AKScript *parentScript=NULL//父级脚本
        );
        ~AKScript();

        //获取最上层的父脚本
        AKScript *getTopParentScript();

        //停止执行脚本
        void stop();

        //是否运行中
        bool isRunning();

        AKScript *getParentScript();

        //获取系统对象
        AKSystem *getSystem();

        //获取核对象
        AKCore *getCore();

        //获取进程对象
        AKProcess *getProcess();

        //设置脚本url
        void setUrl(const int8 *url);

        //获取脚本路径名
        const int8 *getUrl();

        //获取脚本目录
        const int8 *getDir();

        //获取脚本名
        const int8 *getName();

        //获取父脚本名
        const int8 *getParentName();

        //获取变量对象
        AKVar *getVar(
                const int8 *name//变量名称
        );

        //获取变量值
        const int8 *getVarValue(
                const int8 *name//变量名称
        );

        //获取当前指令
        AKOrder *getCurOrder();

        //获取指令状态
        uint32 getOrderStatus();

        //设置指令状态
        void setOrderStatus(uint32 orderStatus);

        //重置脚本环境
        void reset();

        //快速重置脚本环境（调用过reset后需要重置时使用）
        void quickReset();

        //设置中间代码
        void setMidCode(const int8 *midCode);

        //执行脚本：返回false代表脚本执行完成，否则代表未完成
        bool run(
                uint32 executeOrderCount=1000//每次执行脚本的指令数，0代表执行到脚本结束为止，除非遇到指令要求暂停
        );

        //执行下一条指令
        void nextOrder();

        //注册指令函数
        void regOrderFunction(
                const int8 *name,//函数名
                AKScriptFunc orderFunc,//函数对象
                const int8 *remark=""//注释
        );

        //获取指令函数的注释
        const int8 *getOrderFunctionRemark(
                const int8 *name//函数名
        );

        //注册指令处理器
        void regHandler(
                AKHandler *handler,//指令处理器
                const int8 *name=NULL//处理器名称
        );

        //获取指令处理器
        AKHandler *getHandler(
                const int8 *name//处理器名称
        );

        //设置指令执行位置
        void setOrderPos(uint32 pos);

        //获取指令总数
        uint32 getOrderCount();

        //启动异常处理机制//一旦捕获异常，将向下找到最近的catch指令位置开始执行
        void startTry();

        //关闭异常处理机制
        void closeTry();

        //处理异常
        bool tryCatch(
                const int8 *error//错误信息
        );

        //获取异常信息
        const int8 *catchError();

        //返回所有API帮助信息
        std::string help();

        //返回所有API语法提示信息（notepad++）
        std::string helpForNotePad();

        //获取调用脚本堆栈数组
        void getCallStackArr(
                std::vector<AKScript *> &callStackArr//调用脚本堆栈数组
        );

        //加载脚本中间代码
        bool loadMidCode();

        //创建脚本函数
        AKScript *createScriptFunc(
                const int8 *scriptUrl//脚本url
        );

        //检测脚本函数有效性
        bool checkScriptFunc(AKScript *scriptFunc);

        //检测脚本函数实例是否被递归调用
        bool checkScriptFuncCanExec(AKScript *scriptFunc);

    private:

        //运行时出错
        void onRunError(const int8 *error);

        //检查参数是否存在并建立实际的变量对象数组
        bool checkParams(
                AKOrder *order//指令对象
        );

        //重置指令处理器
        void resetOrderHandler();

        //获取由脚本中func创建的脚本函数
        AKScript *getScriptFunc(
                const int8 *funcVarName//脚本函数句柄变量名(由脚本中func api创建）
        );

    private:
        AKSystem *mSystem;
        AKCore *mCore;
        AKProcess *mProcess;
        AKScript *mParentScript;//父级脚本
        ///////////////////////////////////////////////////////////////////////////////////////////
        bool mbIsNeedReset;//是否需要完全重置
        bool mIsLoadMidCode;//是否加载中间代码
        ///////////////////////////////////////////////////////////////////////////////////////////
        AKHandlerMap mHandlerMap;
        AKHandlerList mHandlerList;
        ///////////////////////////////////////////////////////////////////////////////////////////
        std::string mUrl;//脚本url
        std::string mDir;//脚本目录
        std::string mName;//脚本名（如：xxxx.aks.method）
        std::string mParentName;//父脚本名（如：xxxx.aks）
        ///////////////////////////////////////////////////////////////////////////////////////////
        AKStringStringMap mOrderInitErrorMap;//指令初始化错误map（一般是变量不存在的错误）
        bool mIsTry;//是否处理异常
        std::string mCatchError;//捕获的异常信息
        ///////////////////////////////////////////////////////////////////////////////////////////
    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        static const uint32 callType_RunScript=0;
        static const uint32 callType_RequestScript=1;
        static const uint32 callType_callScript=2;
        uint32 mCallType;//脚本的调用类型
        ///////////////////////////////////////////////////////////////////////////////////////////
        container::AKBuf mDataBuf;//传送给脚本使用的参数buf（传参用）
        container::AKBuf mDataPosBuf;//传送给脚本使用的参数位置buf，对应mDataBuf（传参用）
        ///////////////////////////////////////////////////////////////////////////////////////////
        AKVar mRtVar;//返回给脚本调用者的变量
        ///////////////////////////////////////////////////////////////////////////////////////////
        bool mIsFullRun;//是否全速执行（不受执行行数限制）
        ///////////////////////////////////////////////////////////////////////////////////////////
        bool mbRun;//是否正在运行
        std::string mCodeTxt;//源代码
        std::string mMidCodeTxt;//中间代码（指令列表xml）
        AKOrderArray mOrderArray;//指令数组
        AKVarMap mVarMap;//变量map
        uint32 mOrderPos;//正在执行的指令位置
        uint32 mOrderStatus;//正在执行的指令状态
        AKOrderMap mOrderDefineMap;//指令定义map
        AKStringStringMap mOrderRemarkMap;//指令注释map
        ///////////////////////////////////////////////////////////////////////////////////////////
        AKScriptMap mScriptFuncMap;//脚本函数map
        AKStringScriptMap mScriptFuncNameMap;//脚本函数名字map
    };
}

#endif //AKSERVER_AKSCRIPT_H
