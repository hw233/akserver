//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKGLOBAL_H
#define AKSERVER_AKGLOBAL_H

#include "ak.h"

//断点类
typedef std::map<uint32,uint32> AKRowIDMap;//行号列表
typedef std::map<std::string,AKRowIDMap> AKBreakPointMap;//断点列表[脚本url,行号列表]

namespace ak
{
    //全局类
    class AKGlobal
    {
    public:
        AKGlobal();
        ~AKGlobal();
        AKDefineSingleton(AKGlobal);
        ///////////////////////////////////////////////////////////////////////////////
        void beginCallScript(AKScript *script);
        void endCallScript(AKScript *script);
        std::string getScriptCallCountInfo();//获取脚本调用次数快照信息
        ///////////////////////////////////////////////////////////////////////////////
        void addBreakPoint(//添加断点
                const int8 *scriptUrl,//脚本url
                uint32 row//断点行号（从0开始）
        );
        void delBreakPoint(//删除断点
                const int8 *scriptUrl,//脚本url
                uint32 row//断点行号（从0开始）
        );
        void delAllBreakPoint();//删除所有断点
        AKRowIDMap *getBreakPointRowIDMap(//获取断点行号列表
                const int8 *scriptUrl//脚本url
        );
        bool isBreakPointExist(//检查断点是否存在
                const int8 *scriptUrl,//脚本url
                uint32 row//断点行号（从0开始）
        );
        ///////////////////////////////////////////////////////////////////////////////
    private:
        ///////////////////////////////////////////////////////////////////////////////
        //boost::timer mTimer;//开机计时器
        boost::posix_time::ptime mT0;//开机时间戳
        ///////////////////////////////////////////////////////////////////////////////
        AKArrMap mArrMap;//全局数组map（脚本使用）
        AKDictMap mDictMap;//全局字典map（脚本使用）
        ///////////////////////////////////////////////////////////////////////////////
    public:
        AKStringStringMap mAPIMap;//全局api字典map（脚本使用）
        std::string mErrorLogFilePath;//错误记录文件路径
        ///////////////////////////////////////////////////////////////////////////////
        AKBreakPointMap mBreakPointMap;//断点列表[脚本url,行号列表]
        ///////////////////////////////////////////////////////////////////////////////
    };
}

#endif //AKSERVER_AKGLOBAL_H
