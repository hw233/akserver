#include "ak.h"
#include "AKGlobal.h"

namespace ak
{
    AKGlobal::AKGlobal()
    {
        mT0=boost::posix_time::microsec_clock::universal_time();
    }

    AKGlobal::~AKGlobal()
    {
        {
            AKArrMap::iterator it=mArrMap.begin();
            while(it!=mArrMap.end())
            {
                delete it->second;
                ++it;
            }
            mArrMap.clear();
        }

        {
            AKDictMap::iterator it=mDictMap.begin();
            while(it!=mDictMap.end())
            {
                delete it->second;
                ++it;
            }
            mDictMap.clear();
        }

    }

    void AKGlobal::addBreakPoint(//添加断点
            const int8 *scriptUrl,//脚本url
            uint32 row//断点行号（从0开始）
    )
    {
        mBreakPointMap[scriptUrl][row]=row;
    }

    void AKGlobal::delBreakPoint(//删除断点
            const int8 *scriptUrl,//脚本url
            uint32 row//断点行号（从0开始）
    )
    {
        AKBreakPointMap::iterator it=mBreakPointMap.find(scriptUrl);
        if(it!=mBreakPointMap.end())
        {
            AKRowIDMap &rowIDMap=it->second;
            rowIDMap.erase(row);
            if(rowIDMap.size()<=0)
            {
                mBreakPointMap.erase(it);
            }
        }
    }

    bool AKGlobal::isBreakPointExist(//检查断点是否存在
            const int8 *scriptUrl,//脚本url
            uint32 row//断点行号（从0开始）
    )
    {
        AKBreakPointMap::iterator it=mBreakPointMap.find(scriptUrl);
        if(it!=mBreakPointMap.end())
        {
            AKRowIDMap &rowIDMap=it->second;
            AKRowIDMap::iterator it2=rowIDMap.find(row);
            if(it2!=rowIDMap.end())
            {
                return true;
            }
        }
        return false;
    }

    void AKGlobal::delAllBreakPoint()
    {
        mBreakPointMap.clear();
    }

    AKRowIDMap *AKGlobal::getBreakPointRowIDMap(//获取断点行号列表
            const int8 *scriptUrl//脚本url
    )
    {
        AKBreakPointMap::iterator it=mBreakPointMap.find(scriptUrl);
        if(it!=mBreakPointMap.end())
        {
            AKRowIDMap &rowIDMap=it->second;
            return &rowIDMap;
        }
        return NULL;
    }

}
