//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKPROCESS_H
#define AKSERVER_AKPROCESS_H

namespace ak
{
    //进程类
    class AKProcess
    {
    public:
        AKProcess(
                AKSystem *system,//系统对象
                AKCore *core,//核对象
                const int8 *scriptUrl//脚本url
        );
        ~AKProcess();

        void retain(//引用
                uint32 count=1//引用次数
        );
        void release();//释放

        void bind(AKProcess *process);//进程绑定（被绑定的进程在本进程死亡后也跟着死亡）
        void rebind(AKProcess *process);//进程解绑

        //执行脚本：返回false代表脚本执行完成，否则代表未完成
        bool run(
                uint32 executeOrderCount=1000//每次执行脚本的指令数，0代表执行到脚本结束为止，除非遇到指令要求暂停
        );

        void stop();//停止执行脚本

        //向该进程发送消息并请求数据
        void request(
                AKProcess *process,//发送者进程
                container::AKBuf **msgDataArray,//msg数据缓冲数组
                uint32 msgDataSize//msg数据缓冲数量
        );

        //检查是否收到进程msg请求数据
        AKProcess *hasRequest(
                container::AKBuf &msg//msg缓冲
        );

        //向该进程返回数据
        void response(
                AKProcess *process,//发送者进程
                container::AKBuf **msgDataArray,//msg数据缓冲数组
                uint32 msgDataSize//msg数据缓冲数量
        );

        //获取向进程返回的response数据
        AKProcess *getResponse(
                container::AKBuf &msg//msg缓冲
        );

        //向该进程发送msg
        void send(
                AKProcess *process,//发送者进程
                container::AKBuf &msg//msg缓冲
        );

        //向该进程发送msg数据缓冲数组
        void send(
                AKProcess *process,//发送者进程
                container::AKBuf **msgDataArray,//msg数据缓冲数组
                uint32 msgDataSize//msg数据缓冲数量
        );

        //接收进程msg
        AKProcess *recv(//接收成功返回进程句柄（使用前需要检查句柄有效性）
                container::AKBuf &msg//msg缓冲
        );

    private:
        AKSystem *mSystem;//系统对象
        AKCore *mCore;//核对象
        boost::recursive_mutex mRefMutex;
        uint32 mRef;//引用计数

    public:
        AKScript mScript;//进程脚本
        AKProcessMap mBindProcessMap;//绑定进程map
        AKProcess *mBindParent;//绑定的父进程

        container::AKBuf mRecvMsgBuf1;//msg缓冲channel 1
        boost::recursive_mutex mRecvMsgBuf1Mutex;

        container::AKBuf mRecvMsgBuf2;//msg缓冲channel 2
        boost::recursive_mutex mRecvMsgBuf2Mutex;

        container::AKBuf mRecvMsgBuf3;//msg缓冲channel 3
        boost::recursive_mutex mRecvMsgBuf3Mutex;

        container::AKDict mProcessVarDict;//进程变量字典

        boost::timer mTimer;
        double mT0;//时间戳（用于pDt api)

        //用于定义表
        std::string mDBName;//数据库名
        std::string mTableName;//表名
        std::string mEngineName;//存储引擎名

        //本次运行开始时间（毫秒）
        double mRunBeginTime;

        bool mIsNoBreakPoint;//是否禁止使用断点
    };
}

#endif //AKSERVER_AKPROCESS_H
