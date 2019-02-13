#include "ak.h"
#include "AKDict.h"
#include "../AKGlobal.h"

namespace ak
{
    namespace container
    {
        AKDictIterator::AKDictIterator()
                :witch(0),
                 dict(NULL)
        {
        }

        AKDict *AKDictIterator::getDict()
        {
            return dict;
        }

        AKDictIterator::operator bool()
        {
            if(witch==1&&it1!=dict->mUint32VarMap.end())
            {
                return true;
            }
            else if(witch==2&&it2!=dict->mVarMap.end())
            {
                return true;
            }
            return false;
        }

        bool AKDictIterator::operator==(AKDictIterator it)
        {
            if(it.dict==dict&&
               it.witch==witch&&
               it.it1==it1&&
               it.it2==it2
                    )
            {
                return true;
            }
            return false;
        }

        void AKDictIterator::operator++()
        {
            if(witch==1)
            {
                ++it1;
                if(it1==dict->mUint32VarMap.end())
                {
                    witch=2;
                    it2=dict->mVarMap.begin();
                }
            }
            else if(witch==2&&it2!=dict->mVarMap.end())
            {
                ++it2;
            }
        }

        void AKDictIterator::operator--()
        {
            if(witch==1)
            {
                if(it1!=dict->mUint32VarMap.begin())
                {
                    --it1;
                }
                else
                {
                    *this=dict->end();
                }
            }
            else if(witch==2)
            {
                if(it2==dict->mVarMap.begin())
                {
                    if(dict->mUint32VarMap.size()>0)
                    {
                        it2=dict->mVarMap.end();
                        witch=1;
                        it1=dict->mUint32VarMap.end();
                        --it1;
                    }
                    else
                    {
                        *this=dict->end();
                    }
                }
                else
                {
                    --it2;
                }
            }
        }

        AKVar *AKDictIterator::val()
        {
            if(witch==1)
            {
                return it1->second;
            }
            else if(witch==2&&it2!=dict->mVarMap.end())
            {
                return it2->second;
            }
            return NULL;
        }

        uint32 AKDictIterator::keyUint32()
        {
            if(witch==1)
            {
                return it1->first;
            }
            else
            {
                return 0;
            }
        }

        const int8 *AKDictIterator::keyString()
        {
            if(witch==2&&it2!=dict->mVarMap.end())
            {
                return it2->first.c_str();
            }
            return NULL;
        }

        AKDict::AKDict(AKCore *core)
                :AKRef(core)
        {
        }

        AKDict::~AKDict()
        {
            clear();
        }

        AKDictIterator AKDict::end()
        {
            AKDictIterator it;
            it.dict=this;
            if(mVarMap.size()>0)
            {
                it.witch=2;
            }
            else
            {
                it.witch=1;
            }
            it.it1=mUint32VarMap.end();
            it.it2=mVarMap.end();
            return it;
        }

        AKDictIterator AKDict::begin()
        {
            AKDictIterator it;
            it.dict=this;
            if(mUint32VarMap.size()>0)
            {
                it.witch=1;
                it.it1=mUint32VarMap.begin();
                it.it2=mVarMap.end();
            }
            else if(mVarMap.size()>0)
            {
                it.witch=2;
                it.it1=mUint32VarMap.end();
                it.it2=mVarMap.begin();
            }
            else
            {
                it.witch=2;
                it.it1=mUint32VarMap.end();
                it.it2=mVarMap.end();
            }
            return it;
        }

        uint32 AKDict::size()
        {
            return mVarMap.size()+mUint32VarMap.size();
        }

        void AKDict::clear()
        {
            {
                AKUint32VarMap::iterator it=mUint32VarMap.begin();
                while(it!=mUint32VarMap.end())
                {
                    delete it->second;
                    ++it;
                }
                mUint32VarMap.clear();
            }

            {
                AKVarMap::iterator it=mVarMap.begin();
                while(it!=mVarMap.end())
                {
                    delete it->second;
                    ++it;
                }
                mVarMap.clear();
            }

        }

        void AKDict::remove(
                const int8 *index//字符串索引
        )
        {
            uint32 uintIndex;
            bool rt=AKCommon::string2UintIndex(index,uintIndex);
            if(rt)
            {
                remove(uintIndex);
            }
            else
            {
                AKVarMap::iterator it=mVarMap.find(index);
                if(it!=mVarMap.end())
                {
                    delete it->second;
                    mVarMap.erase(it);
                }
            }
        }

        void AKDict::remove(
                uint32 index//数值索引
        )
        {
            AKUint32VarMap::iterator it=mUint32VarMap.find(index);
            if(it==mUint32VarMap.end())return;

            delete it->second;
            mUint32VarMap.erase(it);
        }

        void AKDict::set(
                const int8 *index,//字符串索引
                const int8 *val,//字符串值
                uint32 bytes//字符串字节数
        )
        {
            uint32 uintIndex;
            bool rt=AKCommon::string2UintIndex(index,uintIndex);
            if(rt)
            {
                set(uintIndex,val);
            }
            else
            {
                AKVar *var=mVarMap[index];
                if(var)
                {
                    var->setValue(val);
                }
                else
                {
                    var=new AKVar(getCore());
                    var->setType(AKVar::typeString);
                    if(bytes==0)var->setValue(val);
                    else var->setValue(std::string(val,bytes));
                    mVarMap[index]=var;
                }
            }
        }

        void AKDict::set(
                const int8 *index,//字符串索引
                double val//数值
        )
        {
            uint32 uintIndex;
            bool rt=AKCommon::string2UintIndex(index,uintIndex);
            if(rt)
            {
                set(uintIndex,val);
            }
            else
            {
                AKVar *var=mVarMap[index];
                if(var)
                {
                    var->setValue(val);
                }
                else
                {
                    var=new AKVar(getCore());
                    var->setType(AKVar::typeNumber);
                    var->setValue(val);
                    mVarMap[index]=var;
                }

            }
        }

        void AKDict::set(
                const int8 *index,//字符串索引
                void *handle//句柄
        )
        {
            uint32 uintIndex;
            bool rt=AKCommon::string2UintIndex(index,uintIndex);
            if(rt)
            {
                set(uintIndex,handle);
            }
            else
            {
                AKVar *var=mVarMap[index];
                if(var)
                {
                    var->setValue(handle);
                }
                else
                {
                    var=new AKVar(getCore());
                    var->setType(AKVar::typeHandle);
                    var->setValue(handle);
                    mVarMap[index]=var;
                }
            }
        }

        void AKDict::set(
                const int8 *index,//字符串索引
                AKVar *var//变量
        )
        {
            if(var->getType()==AKVar::typeNumber)
            {
                set(index,var->toNumber());
            }
            else if(var->getType()==AKVar::typeString)
            {
                set(index,var->toString(),var->toStringSize());
            }
            else if(var->getType()==AKVar::typeHandle)
            {
                set(index,var->toHandle());
            }
        }

        void AKDict::set(
                uint32 index,//数值索引
                double val//数值
        )
        {
            AKVar *var=NULL;
            AKUint32VarMap::iterator it=mUint32VarMap.find(index);
            if(it!=mUint32VarMap.end())
            {
                var=it->second;
            }
            if(var)
            {
                var->setValue(val);
            }
            else
            {
                var=new AKVar(getCore());
                var->setType(AKVar::typeNumber);
                var->setValue(val);
                mUint32VarMap[index]=var;
            }

        }

        void AKDict::set(
                uint32 index,//数值索引
                const int8 *val,//字符串值
                uint32 bytes//字符串字节数
        )
        {
            AKVar *var=NULL;
            AKUint32VarMap::iterator it=mUint32VarMap.find(index);
            if(it!=mUint32VarMap.end())
            {
                var=it->second;
            }
            if(var)
            {
                var->setValue(val);
            }
            else
            {
                var=new AKVar(getCore());
                var->setType(AKVar::typeString);
                if(bytes==0)var->setValue(val);
                else var->setValue(std::string(val,bytes));
                mUint32VarMap[index]=var;
            }

        }

        void AKDict::set(
                uint32 index,//数值索引
                void *handle//句柄
        )
        {
            AKVar *var=NULL;
            AKUint32VarMap::iterator it=mUint32VarMap.find(index);
            if(it!=mUint32VarMap.end())
            {
                var=it->second;
            }
            if(var)
            {
                var->setValue(handle);
            }
            else
            {
                var=new AKVar(getCore());
                var->setType(AKVar::typeHandle);
                var->setValue(handle);
                mUint32VarMap[index]=var;
            }

        }

        void AKDict::set(
                uint32 index,//数值索引
                AKVar *var//变量
        )
        {
            if(var->getType()==AKVar::typeNumber)
            {
                set(index,var->toNumber());
            }
            else if(var->getType()==AKVar::typeString)
            {
                set(index,var->toString(),var->toStringSize());
            }
            else if(var->getType()==AKVar::typeHandle)
            {
                set(index,var->toHandle());
            }
        }

        AKVar *AKDict::get(
                const int8 *index//字符串索引
        )
        {
            uint32 uintIndex;
            bool rt=AKCommon::string2UintIndex(index,uintIndex);
            if(rt)
            {
                return get(uintIndex);
            }
            else
            {
                AKVarMap::iterator it=mVarMap.find(index);
                if(it==mVarMap.end())return NULL;
                return it->second;
            }
        }

        AKVar *AKDict::get(
                uint32 index//数值索引
        )
        {
            AKUint32VarMap::iterator it=mUint32VarMap.find(index);
            if(it!=mUint32VarMap.end())
            {
                return it->second;
            }
            return NULL;
        }

    }
}
