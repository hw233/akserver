//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKREF_H
#define AKSERVER_AKREF_H

namespace ak
{
    //引用基类
    class AKRef
    {
    public:
        AKRef(AKCore *core);
        virtual ~AKRef();

        AKCore *getCore();

        void retain(AKVar *var/*,int32 debugID*/);//引用
        void release(AKVar *var/*,int32 debugID*/);//释放
        uint32 getRefCount();//获取引用次数

    protected:
        uint32 mRefCount;//引用计数
        AKCore *mCore;
        typedef std::map<AKVar *,uint32> VarRefCountMap;
        VarRefCountMap mVarRefCountMap;
        //AKStringList mDebugStrList;
    };
}

#endif //AKSERVER_AKREF_H
