#include "ak.h"
#include "AKArr.h"
#include "../AKGlobal.h"

namespace ak
{
    namespace container
    {
        AKArr::AKArr(AKCore *core)
                :AKRef(core)
        {
        }

        AKArr::~AKArr()
        {
            clear();
        }

        /*void AKArr::addChild(AKArr *arr)
        {
            arr->mParent=this;
            mChildren[arr]=arr;
        }*/

        uint32 AKArr::size()
        {
            return mVarArray.size();
        }

        void AKArr::clear()
        {
            {
                AKVarArray::iterator it=mVarArray.begin();
                while(it!=mVarArray.end())
                {
                    delete *it;
                    ++it;
                }
                mVarArray.clear();
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

            //{
            //	//从父移除
            //	if(mParent)
            //	{
            //		mParent->mChildren.erase(this);
            //	}

            //	//销毁所有子
            //	AKGlobal &global=AKGlobal::getSingleton();
            //	AKArrMap::iterator it=mChildren.begin();
            //	while(it!=mChildren.end())
            //	{
            //		global.destroyArr(it->second);
            //		++it;
            //	}
            //	mChildren.clear();
            //}

        }

        void AKArr::push(
                const int8 *val,//值
                uint32 bytes//字符串字节数
        )
        {
            AKVar *var=new AKVar(getCore());
            var->setType(AKVar::typeString);
            if(bytes==0)var->setValue(val);
            else var->setValue(std::string(val,bytes));
            mVarArray.push_back(var);
        }

        void AKArr::push(
                double val//值
        )
        {
            AKVar *var=new AKVar(getCore());
            var->setType(AKVar::typeNumber);
            var->setValue(val);
            mVarArray.push_back(var);
        }

        void AKArr::push(
                void *handle//值
        )
        {
            AKVar *var=new AKVar(getCore());
            var->setType(AKVar::typeHandle);
            var->setValue(handle);
            mVarArray.push_back(var);
        }

        void AKArr::push(
                AKVar *var//变量
        )
        {
            if(var->getType()==AKVar::typeNumber)
            {
                push(var->toNumber());
            }
            else if(var->getType()==AKVar::typeString)
            {
                push(var->toString(),var->toStringSize());
            }
            else if(var->getType()==AKVar::typeHandle)
            {
                push(var->toHandle());
            }
        }

        void AKArr::remove(
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
                delete mVarMap[index];
                mVarMap.erase(index);
            }
        }

        void AKArr::remove(
                uint32 index//数值索引
        )
        {
            if(index<mVarArray.size())
            {
                delete mVarArray[index];
                mVarArray.erase(mVarArray.begin()+index);
            }
        }

        void AKArr::set(
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

        void AKArr::set(
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

        void AKArr::set(
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

        void AKArr::set(
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

        void AKArr::set(
                uint32 index,//数值索引
                double val//数值
        )
        {
            if(index>=mVarArray.size())
            {
                mVarArray.resize(index+1);
            }
            AKVar *var=mVarArray[index];
            if(var)
            {
                var->setValue(val);
            }
            else
            {
                var=new AKVar(getCore());
                var->setType(AKVar::typeNumber);
                var->setValue(val);
                mVarArray[index]=var;
            }

        }

        void AKArr::set(
                uint32 index,//数值索引
                const int8 *val,//字符串值
                uint32 bytes//字符串字节数
        )
        {
            if(index>=mVarArray.size())
            {
                mVarArray.resize(index+1);
            }
            AKVar *var=mVarArray[index];
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
                mVarArray[index]=var;
            }

        }

        void AKArr::set(
                uint32 index,//数值索引
                void *handle//句柄
        )
        {
            if(index>=mVarArray.size())
            {
                mVarArray.resize(index+1);
            }
            AKVar *var=mVarArray[index];
            if(var)
            {
                var->setValue(handle);
            }
            else
            {
                var=new AKVar(getCore());
                var->setType(AKVar::typeHandle);
                var->setValue(handle);
                mVarArray[index]=var;
            }

        }

        void AKArr::set(
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

        AKVar *AKArr::get(
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

        AKVar *AKArr::get(
                uint32 index//数值索引
        )
        {
            if(index>=mVarArray.size())return NULL;
            AKVar *var=mVarArray[index];
            return var;
        }

        void AKArr::fromSplitString(
                const int8 *str,//源字符串
                const int8 *spliter//分割字符串
        )
        {
            //分割成数组
            int32 spliterLen=strlen(spliter);
            std::string srcStr=str;
            int32 pos=0;
            while(true)
            {
                int32 pos2=srcStr.find(spliter,pos);
                if(pos2>=0)
                {
                    push(srcStr.substr(pos,pos2-pos).c_str());
                    pos=pos2+spliterLen;
                    if(pos>=srcStr.size())break;
                }
                else
                {
                    push(srcStr.substr(pos).c_str());
                    break;
                }
            }
        }

        //根据值查找并返回字符串键
        bool AKArr::findStringKey(
                std::string &key,//键
                AKVar *val//值
        )
        {
            AKVarMap::iterator it=mVarMap.begin();
            while(it!=mVarMap.end())
            {
                AKVar *curVal=it->second;
                //if(curVal->getType()==val->getType())//类型相同
                //{
                switch(curVal->getType())
                {
                    case AKVar::typeNumber:
                    {
                        if(curVal->toNumber()==val->toNumber())
                        {
                            key=it->first;
                            return true;
                        }
                        break;
                    }
                    case AKVar::typeString:
                    {
                        if(strcmp(curVal->toString(),val->toString())==0)
                        {
                            key=it->first;
                            return true;
                        }
                        break;
                    }
                    case AKVar::typeHandle:
                    {
                        if(((int8 *)curVal->toHandle())==((int8 *)val->toHandle()))
                        {
                            key=it->first;
                            return true;
                        }
                        break;
                    }
                }
                //}
                ++it;
            }
            return false;
        }

        //根据值查找并返回uint32键
        bool AKArr::findUint32Key(
                uint32 &key,//键
                AKVar *val//值
        )
        {
            uint32 c=mVarArray.size();
            for(uint32 i=0;i<c;++i)
            {
                AKVar *curVal=mVarArray[i];
                //if(curVal->getType()==val->getType())//类型相同
                //{
                switch(curVal->getType())
                {
                    case AKVar::typeNumber:
                    {
                        if(curVal->toNumber()==val->toNumber())
                        {
                            key=i;
                            return true;
                        }
                        break;
                    }
                    case AKVar::typeString:
                    {
                        if(curVal->toStdString()==val->toStdString())
                        {
                            key=i;
                            return true;
                        }
                        break;
                    }
                    case AKVar::typeHandle:
                    {
                        if(((int8 *)curVal->toHandle())==((int8 *)val->toHandle()))
                        {
                            key=i;
                            return true;
                        }
                        break;
                    }
                }
                //}
            }
            return false;
        }

    }
}
