//
// Created by Sunny Yang on 2019-02-12.
//

#include "ak.h"
#include "AKRef.h"

namespace ak
{
    AKRef::AKRef(AKCore *core)
            :mRefCount(0),
             mCore(core)
    {
    }

    AKRef::~AKRef()
    {
    }

    AKCore *AKRef::getCore()
    {
        return mCore;
    }

    void AKRef::retain(AKVar *var/*,int32 debugID*/)
    {
        ////////////////////////////////////////////////////////////////////////////
        ////AK_DEBUG//测试代码
        //int8 tmp[1024];
        //sprintf(tmp,"%ld(%s)",debugID,var->mName.c_str());
        //mDebugStrList.push_back(tmp);
        if(mVarRefCountMap.find(var)==mVarRefCountMap.end())mVarRefCountMap[var]=0;
        ++mVarRefCountMap[var];
        ////////////////////////////////////////////////////////////////////////////

        ++mRefCount;
    }

    void AKRef::release(AKVar *var/*,int32 debugID*/)
    {
        ////////////////////////////////////////////////////////////////////////////
        ////AK_DEBUG//测试代码
        //int8 tmp[1024];
        //sprintf(tmp,"%ld(%s)",debugID,var->mName.c_str());
        //mDebugStrList.push_back(tmp);
        VarRefCountMap::iterator it=mVarRefCountMap.find(var);
        if(it==mVarRefCountMap.end())
        {
            /*std::string debugStr="";
            AKStringList::iterator it=mDebugStrList.begin();
            while(it!=mDebugStrList.end())
            {
                debugStr+=" ";
                debugStr+=*it;
                ++it;
            }
            AKCommon::log("ak__testRefLog.txt","error release");
            AKCommon::log("ak__testRefLog.txt",debugStr.c_str());
            AKCommon::log("ak__testRefLog.txt","\r\n");*/
            return;
        }
        else
        {
            uint32 &refCount=it->second;
            --refCount;
            if(refCount==0)mVarRefCountMap.erase(it);
        }
        ////////////////////////////////////////////////////////////////////////////

        --mRefCount;
        if(mRefCount==0)//垃圾回收
        {
            AKRefMap::iterator it=mCore->mRefMap.find(this);
            mCore->mRefMap.erase(it);
            delete this;
        }
    }

    uint32 AKRef::getRefCount()
    {
        return mRefCount;
    }

}

