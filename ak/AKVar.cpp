#include "ak.h"
#include "AKVar.h"

namespace ak
{
    AKVar::AKVar(AKCore *core)
            :mCore(core),
             type(0),
             strVal(""),
             numVal(0.0),
             handleVal(NULL),
             initVal(""),
             initValType(0),
             mRefVar(NULL),
             mIsTmpRef(false)
    {
    }

    AKVar::~AKVar()
    {
        //处理引用计数
        tryReleaseHandle(handleVal/*,6666*/);
        handleVal=NULL;

        //取消引用
        unRef();

        {
            //删除被其它变量引用的信息
            AKVarVarMap::iterator it=mRefParent.begin();
            while(it!=mRefParent.end())
            {
                AKVar *curVar=it->second;
                ++it;
                curVar->unRef();
            }
        }
    }

    //返回变量实例
    AKVar *AKVar::getInst()
    {
        if(mRefVar)
        {
            return mRefVar->getInst();
        }
        return this;
    }

    //引用其它变量
    void AKVar::ref(AKVar *var,bool isTmpRef)
    {
        //取消旧引用
        unRef();

        setType(AKVar::typeRef);
        AKVar *inst=var->getInst();
        mRefVar=inst;
        mRefVar->mRefParent[this]=this;
        mIsTmpRef=isTmpRef;
    }

    //取消引用
    void AKVar::unRef()
    {
        if(mRefVar)
        {
            mRefVar->mRefParent.erase(this);
            mRefVar=NULL;
        }
    }

    //清除类型和值
    void AKVar::clearType()
    {
        //处理引用计数
        tryReleaseHandle(handleVal/*,5555*/);
        handleVal=NULL;
        mIsTmpRef=false;

        type=0;
        strVal="";
        numVal=0;
        initVal="";
        initValType=0;

        //取消引用
        unRef();
    }

    //初始化值
    void AKVar::initValue(
            const int8 *val,//初始值
            uint32 valType//初始类型
    )
    {
        //处理引用计数
        tryReleaseHandle(handleVal/*,4444*/);
        handleVal=NULL;

        initValType=valType;
        initVal=val;
        setType(initValType);
        setValue(val);
    }

    //重置为初始值
    void AKVar::resetValue()
    {
        //处理引用计数
        tryReleaseHandle(handleVal/*,3333*/);
        handleVal=NULL;
        mIsTmpRef=false;

        strVal="";
        numVal=0;
        type=initValType;//重置为初始类型
        setValue(initVal.c_str());//重置为初始值

        //取消引用
        unRef();
    }

    void AKVar::tryRetainHandle(void *handle/*,int32 debugID*/)
    {
        if(!handle)return;
        AKRef *ref=mCore->checkRef(handle);
        if(ref)ref->retain(this/*,debugID*/);
    }

    void AKVar::tryReleaseHandle(void *handle/*,int32 debugID*/)
    {
        if(!handle)return;
        AKRef *ref=mCore->checkRef(handle);
        if(ref)ref->release(this/*,debugID*/);
    }

    //设置值
    void AKVar::setValue(void *handle)
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            mRefVar->setValue(handle);
            return;
        }

        if(type!=AKVar::typeHandle)
        {
            throw "非句柄类型不能转成句柄类型";
        }

        //处理引用计数
        tryReleaseHandle(handleVal/*,2222*/);
        tryRetainHandle(handle/*,1111*/);

        //赋值
        handleVal=handle;
    }

    //设置值
    void AKVar::setValue(const int8 *val)
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            mRefVar->setValue(val);
            return;
        }

        if(type==AKVar::typeHandle)
        {
            throw "句柄类型不能转成字符串类型";
        }

        if(type==typeString)strVal=val;
        else if(type==typeNumber)
        {
            numVal=atof(val);
        }
    }

    //设置值
    void AKVar::setValue(const std::string &val)
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            mRefVar->setValue(val);
            return;
        }

        if(type==AKVar::typeHandle)
        {
            throw "句柄类型不能转成字符串类型";
        }

        if(type==typeString)strVal=val;
        else if(type==typeNumber)
        {
            numVal=atof(val.c_str());
        }
    }

    //设置值
    void AKVar::setValue(double val)
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            mRefVar->setValue(val);
            return;
        }

        if(type==AKVar::typeHandle)
        {
            throw "句柄类型不能转成数值类型";
        }

        if(type==typeString)
        {
            int64 intVal=val;
            int8 tmp[1024];
            if(val==intVal)//没有小数部分
            {
                sprintf(tmp,"%lld",intVal);
                /*std::stringstream ss;
                ss<<intVal;
                strVal=ss.str();*/
            }
            else//存在小数部分
            {
                sprintf(tmp,"%lf",val);
                /*std::stringstream ss;
                ss<<val;
                strVal=ss.str();*/
            }
            strVal=tmp;
        }
        else if(type==typeNumber)
        {
            numVal=val;
        }
    }

    //设置值
    void AKVar::setValue(AKVar *var)
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            mRefVar->setValue(var);
            return;
        }

        setType(var->getType());
        if(var->getType()==AKVar::typeNumber)
        {
            setValue(var->toNumber());
        }
        else if(var->getType()==AKVar::typeString)
        {
            setValue(var->toStdString());
        }
        else if(var->getType()==AKVar::typeHandle)
        {
            setValue(var->toHandle());
        }
    }

    //设置变量类型（只能设置一次，第二次设置时无效）
    void AKVar::setType(uint32 type)
    {
        if(this->type==0&&(type==typeString||type==typeNumber||type==typeHandle||type==typeRef))
        {
            this->type=type;
        }
        else if(this->type==typeRef)
        {
            if(getInst()!=this)
            {
                getInst()->setType(type);
            }
        }
    }

    //是否为临时引用
    bool AKVar::isTmpRef()
    {
        return mIsTmpRef;
    }

    //是否为引用
    bool AKVar::isRef()
    {
        if(type==AKVar::typeRef)return true;
        return false;
    }

    //返回变量类型
    uint32 AKVar::getType()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->getType();
        }

        return type;
    }

    //返回字符串
    const int8 *AKVar::toString()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toString();
        }

        if(type==typeHandle)
        {
            throw "句柄类型不能转成字符串类型";
        }

        if (type==typeString)
        {
            return strVal.c_str();
        }
        else if(type==typeNumber)
        {
            int64 intVal=numVal;
            int8 tmp[1024];
            if(numVal==intVal)//没有小数部分
            {
                //sprintf(tmp,"%I64d",intVal);
                sprintf(tmp,"%lld",intVal);
            }
            else//存在小数部分
            {
                sprintf(tmp,"%lf",numVal);
                uint32 c = strlen(tmp)-1;
                for (;c>0; --c)
                {
                    int8 ch = tmp[c];
                    if (ch == '0' || ch == '.')
                    {
                        tmp[c] = '\0';
                    }
                    else
                    {
                        break;
                    }
                }
            }
            strVal=tmp;

            return strVal.c_str();
        }
        strVal="";
        return "";
    }

    //返回字符串
    std::string &AKVar::toStdString()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toStdString();
        }

        if(type==typeHandle)
        {
            throw "句柄类型不能转成字符串类型";
        }

        if (type==typeString)
        {
            return strVal;
        }
        else if(type==typeNumber)
        {
            int64 intVal=numVal;
            int8 tmp[1024];
            if(numVal==intVal)//没有小数部分
            {
                //sprintf(tmp,"%I64d",intVal);
                sprintf(tmp,"%lld",intVal);
            }
            else//存在小数部分
            {
                sprintf(tmp,"%lf",numVal);
            }
            strVal=tmp;

            return strVal;
        }
        strVal="";
        return strVal;
    }

    //返回字符串尺寸
    uint32 AKVar::toStringSize()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toStringSize();
        }

        if(type==typeHandle)
        {
            throw "句柄类型不能转成字符串类型";
        }

        toString();
        return strVal.size();
    }

    //返回数值
    double AKVar::toNumber()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toNumber();
        }

        if(type==typeHandle)
        {
            throw "句柄类型不能转成数值类型";
        }

        if (type==typeString)
        {
            return atof(strVal.c_str());
        }
        else if (type==typeNumber)
        {
            return numVal;
        }
        return 0;
    }

    //返回整数值
    int32 AKVar::toInt()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toInt();
        }

        if(type==typeHandle)
        {
            throw "句柄类型不能转成数值类型";
        }

        if (type==typeString)
        {
            return atoi(strVal.c_str());
        }
        else if (type==typeNumber)
        {
            return numVal;
        }
        return 0;
    }

    //返回无符号整数值
    uint32 AKVar::toUint()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toUint();
        }

        if(type==typeHandle)
        {
            throw "句柄类型不能转成数值类型";
        }

        if (type==typeString)
        {
            return atoi(strVal.c_str());
        }
        else if (type==typeNumber)
        {
            return numVal;
        }
        return 0;
    }

    void *AKVar::toHandle()
    {
        if(type==AKVar::typeRef)//引用了其它变量
        {
            if(!mRefVar)throw "空引用";
            assert(mRefVar);
            return mRefVar->toHandle();
        }

        if(type!=typeHandle)
        {
            throw "转成句柄类型失败";
        }

        return handleVal;
    }

}
