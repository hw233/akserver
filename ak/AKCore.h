//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKCORE_H
#define AKSERVER_AKCORE_H

namespace ak
{
    //进程信息
    class AKProcessInfo
    {
    public:
        AKProcessInfo()
                :coreID(0),
                 process(NULL)
        {
        }
        uint32 coreID;//核id
        AKProcess *process;//进程
    };

    //核类
    class AKCore
    {
    public:
        friend class AKSystem;
        AKCore(
                AKSystem *system,//系统对象
                uint32 id//核id
        );
        ~AKCore();
        uint32 getID();//获取核id
        boost::asio::io_service &getIO();
        void quit();//退出
        bool isQuit();
        bool checkProcess(AKProcess *process);//检查进程有效性
        void asynCreateProcess(AKProcessInfo info);
        void asynDestroyProcess(AKProcess *process);

        //检查client有效性
        bool checkClient(net::AKClient *client);

        //创建client对象
        net::AKClient *createClient(
                AKProcess *process,//进程
                const int8 *domain,//域名或ip
                uint32 port,//端口
                uint32 recvBufSize=1024*1024//接收缓冲尺寸
        );

        //删除client对象
        void removeClient(net::AKClient *client);

        //检查server有效性
        bool checkServer(net::AKServer *server);

        //创建server对象
        net::AKServer *createServer(
                AKProcess *process,//进程
                const int8 *bindIP,//绑定ip或域名
                uint32 port,//端口
                uint32 clientMaxCount,//最大client数量
                uint32 keepAcceptCount,//保持accept数量
                uint32 recvBufSize//接收缓冲尺寸
        );

        //创建server对象
        net2::AKServerForScript *createServer2(
                const int8 *bindDomain,//绑定ip或域名
                uint16 port//端口
        );

        //检查server有效性
        bool checkServer2(net2::AKServerForScript *server);

        //创建client对象
        net2::AKClientForScript *createClient2();

        //销毁client对象
        void destroyClient2(net2::AKClientForScript *client);

        //检查client有效性
        bool checkClient2(net2::AKClientForScript *client);

        ////////////////////////////////////////////////////////////////////////////
        ak::container::AKBuf *createBuf();//创建缓冲
        bool checkBuf(AKRef *ref);//检查缓冲是否有效
        container::AKArr *createArr();//创建数组
        bool checkArr(AKRef *ref);//检查数组有效性
        container::AKDict *createDict();//创建字典
        bool checkDict(AKRef *ref);//检查字典有效性
        void destroyRef(AKRef *ref);//销毁引用
        AKRef *checkRef(void *handle);//检查句柄是否为引用对象，是则返回，否则返回NULL
        ////////////////////////////////////////////////////////////////////////////
        ak::container::AKStruct *createStruct();//创建结构体
        bool checkStruct(ak::container::AKStruct *structObj);//检查结构体是否有效
        void destroyStruct(ak::container::AKStruct *structObj);//销毁结构体
        ////////////////////////////////////////////////////////////////////////////

        std::string getRunTimeInfo();//获取核运行时信息快照

    private:
        void update(double dt);
        void asynUpdate();
        void asynQuit();
        void asynStopAllProcessScript();//停止所有进程脚本

    private:
        bool mIsDestroy;//是否被销毁
        AKSystem *mSystem;//系统对象
        bool mIsQuit;//是否退出
        boost::timer mUpdateTimer;//更新计时器
        uint32 mID;//核id
        boost::asio::io_service mIO;
        AKProcessInfoMap mProcessInfoMap;//全局进程map
        AKProcessInfoMap mCoreProcessInfoMap;//核进程map

        AKClientMap mClientMap;//client对象map
        AKServerMap mServerMap;//server对象map

        AKServerMap2 serverMap;
        AKClientMap2 clientMap;


    public:
        ///////////////////////////////////////////////////////////////////////////////
        container::AKDict mGlobalVarDict;//全局变量字典
        ///////////////////////////////////////////////////////////////////////////////
        AKRefMap mRefMap;//引用列表（用于自动垃圾回收）
        ///////////////////////////////////////////////////////////////////////////////
        typedef std::map<ak::container::AKStruct *,ak::container::AKStruct *> AKStructMap;
        AKStructMap structMap;
        ///////////////////////////////////////////////////////////////////////////////
        container::AKStaticMem mStaticMem;//临时空间（提供需要临时小内存需求的场合使用）
        ///////////////////////////////////////////////////////////////////////////////
    };
}

#endif //AKSERVER_AKCORE_H
