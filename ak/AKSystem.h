//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSYSTEM_H
#define AKSERVER_AKSYSTEM_H

namespace ak
{
    //系统类
    class AKSystem
    {
    public:
        AKSystem(
                uint32 coreCount=2//核数
        );
        virtual ~AKSystem();
        boost::asio::io_service &getIO();
        void quit();//退出
        uint32 getCoreCount();//获取核数
        uint32 getFreeCoreID();//获取空闲核id

        AKProcess *createProcess(//创建进程
                const int8 *scriptUrl,//脚本url
                uint32 coreID=0,//核id（0=自动分配）
                container::AKBuf *buf=NULL//数据缓冲
        );

        void destroyProcess(//销毁进程
                AKProcess *process//进程对象
        );

        void coreThread(//核线程函数
                uint32 coreID//核id
        );

        void onPreError(//错误预处理
                AKScript *script,
                const int8 *error
        );

    private:
        void asynStart();
        void asynQuit();
        void asynUpdate();

    public:
        //更新
        virtual void update(double dt);

        //运行时出错信息重定向（通过继承并重写此方法，可重定向错误信息）
        virtual void onRunError(const int8 *error)=0;

        //运行时异常信息重定向（通过继承并重写此方法，可重定向错误信息）
        virtual void onRunCatch(const int8 *error)=0;

        //跟踪打印信息
        virtual void onTrace(const int8 *msg)=0;

        //注册自定义脚本指令处理器
        virtual void onRegHandler(AKScript *script)=0;

    public:
        int32 mAutoSleepTime;//进程自动休眠时间（毫秒）//设置为>=0时，适应避免大量占用CPU的场合

    private:
        bool mIsQuit;//是否退出
        boost::timer mUpdateTimer;//更新计时器
        boost::asio::io_service mIO;
        uint32 mCoreCount;//核数量（额外预留一个系统任务进程使用的核（该核ID为最大））
        AKCoreArray mCoreArray;//核数组（id从1开始）
        AKThreadList mThreadList;//线程列表
    };
}

#endif //AKSERVER_AKSYSTEM_H
