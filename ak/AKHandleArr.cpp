#include "ak.h"
#include "AKScript.h"
#include "AKHandleArr.h"
#include "AKGlobal.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleArr::AKHandleArr()
            :mParent(NULL)
    {
    }

    AKHandleArr::~AKHandleArr()
    {
        reset();
    }

    void AKHandleArr::reset()
    {
    }

    void AKHandleArr::init(AKScript *script)
    {
        AKHandler::init(script);

        AKQuickBindScriptFunc("arrDestroy",&AKHandleArr::arrDestroy,"arrDestroy(arr);//销毁数组");
        AKQuickBindScriptFunc("arr",&AKHandleArr::arrCreate,"arr=arr([e1,e2,e3,...]);//创建数组");
        AKQuickBindScriptFunc("gArrCreate",&AKHandleArr::arrCreate,"//同arr()，已淘汰");
        AKQuickBindScriptFunc("pArrCreate",&AKHandleArr::arrCreate,"//同arr()，已淘汰");
        AKQuickBindScriptFunc("arrCreate",&AKHandleArr::arrCreate,"//同arr()，已淘汰");
        AKQuickBindScriptFunc("arrSize",&AKHandleArr::arrSize,"size=arrSize(arr);//获取数组尺寸");
        AKQuickBindScriptFunc("arrClear",&AKHandleArr::arrClear,"arrClear(arr);//清空数组");
        AKQuickBindScriptFunc("arrPush",&AKHandleArr::arrPush,"arr=arrPush(arr,val1,val2,...);//数组压入元素值");
        AKQuickBindScriptFunc("arrSet",&AKHandleArr::arrSet,"arrSet(arr,key,val);//设置数组元素值");
        AKQuickBindScriptFunc("arrGet",&AKHandleArr::arrGet,"val=arrGet(arr,key);//获取数组元素值");
        AKQuickBindScriptFunc("arrGetRef",&AKHandleArr::arrGetRef,"ref=arrGetRef(arr,key);//获取数组元素引用");
        AKQuickBindScriptFunc("strSplit",&AKHandleArr::strSplit, "arr=strSplit(arr,str,\",\");//字符串分割为数组");
        AKQuickBindScriptFunc("split",&AKHandleArr::strSplit, "arr=split(arr,str,splitStr);//字符串分割为数组，splitStr为分割符");
        AKQuickBindScriptFunc("join",&AKHandleArr::strJoin, "str=join(arr,joinStr);//数组连接为字符串，joinStr为连接符");
        AKQuickBindScriptFunc("isArr",&AKHandleArr::isArr, "isArr=isArr(var);//判断是否为数组");

        AKQuickBindScriptFunc("arrSub",&AKHandleArr::arrSub,"arrDest=arrSub(arrSrc,beginPos,size);//从下标beginPos开始截取size个元素并返回新的数组");
        AKQuickBindScriptFunc("pArrSub",&AKHandleArr::arrSub,"//同arrSub，已淘汰");
        AKQuickBindScriptFunc("gArrSub",&AKHandleArr::arrSub,"//同arrSub，已淘汰");

        AKQuickBindScriptFunc("arrRemove",&AKHandleArr::arrRemove,"arrRemove(arr,key);//数组删除元素");
        AKQuickBindScriptFunc("arrFindNumberKey",&AKHandleArr::arrFindNumberKey,"numKey=arrFindNumberKey(arr,val);//数组查找元素数值下标");
        AKQuickBindScriptFunc("arrFindStringKey",&AKHandleArr::arrFindStringKey,"stingKey=arrFindStringKey(arr,val);//数组查找元素字符串下标");
    }

    //判断是否为数组
    bool AKHandleArr::isArr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        bool isArr=false;
        if(order->getParam(1)->getType()==AKVar::typeHandle)
        {
            container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
            if(script->getCore()->checkArr(arr))isArr=true;
        }

        if(isArr)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //数组查找元素字符串下标
    bool AKHandleArr::arrFindStringKey(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        AKVar *val=order->getParam(2);
        std::string key;
        if(arr->findStringKey(key,val))
        {
            out->setValue(key);
        }
        else
        {
            out->setValue((const int8 *)"");
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //数组查找元素数值下标
    bool AKHandleArr::arrFindNumberKey(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        AKVar *val=order->getParam(2);
        uint32 key;
        if(arr->findUint32Key(key,val))
        {
            out->setValue((double)key);
        }
        else
        {
            out->setValue((double)-1);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //销毁全局数组
    bool AKHandleArr::arrDestroy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        //销毁全局数组
        container::AKArr *arrHandle=(container::AKArr *)order->getParam(1)->toHandle();
        script->getCore()->destroyRef(arrHandle);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建数组
    bool AKHandleArr::arrCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKArr *arr=script->getCore()->createArr();

        //添加数组元素
        for(uint32 i=1;order->getParam(i);++i)
        {
            arr->push(order->getParam(i));
        }

        out->setValue((void *)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //截取并返回数组
    bool AKHandleArr::arrSub(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        //源数组
        container::AKArr *arrSrc=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arrSrc))
        {
            onRunError(order,"无效源数组");
            return false;//暂停执行脚本
        }

        //截取开始位置
        uint32 beginPos=0;
        if (order->getParam(2))beginPos=order->getParam(2)->toUint();

        //截取尺寸
        uint32 size=0;
        if (order->getParam(3))size=order->getParam(2)->toUint();
        if (size==0)size=arrSrc->size();

        //目标数组
        container::AKArr *arrDest=script->getCore()->createArr();

        //截取并填充到新数组
        if(beginPos<arrSrc->size())
        {
            if((beginPos+size)>arrSrc->size())
            {
                size=arrSrc->size()-beginPos;
            }

            for(uint32 i=0;i<size;++i)
            {
                uint32 curIndex=beginPos+i;
                AKVar *var;
                var=arrSrc->get(curIndex);
                if(var)
                {
                    arrDest->set(i,var);
                }
            }
        }

        out->setValue((void *)arrDest);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //清空数组
    bool AKHandleArr::arrClear(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        arr->clear();
        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取数组尺寸
    bool AKHandleArr::arrSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        out->setValue((double)arr->size());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //数组压入元素值
    bool AKHandleArr::arrPush(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        uint32 m=order->varParams.size();
        for(uint32 i=2;i<m;++i)
        {
            arr->push(order->getParam(i));
        }

        out->setValue((void *)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //数组删除元素
    bool AKHandleArr::arrRemove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            arr->remove(index->toUint());
        }
        else if(index->getType()==AKVar::typeString)
        {
            arr->remove(index->toString());
        }
        else//非法变量
        {
            onRunError(order,"无效索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置数组元素值
    bool AKHandleArr::arrSet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            arr->set(index->toUint(),order->getParam(3));
        }
        else if(index->getType()==AKVar::typeString)
        {
            arr->set(index->toString(),order->getParam(3));
        }
        else//非法变量
        {
            onRunError(order,"无效索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取数组元素引用
    bool AKHandleArr::arrGetRef(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        AKVar *var=NULL;
        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            var=arr->get(index->toUint());
            if(!var)
            {
                onRunError(order,"数组索引查找失败");
                return false;//暂停执行脚本
            }
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=arr->get(index->toString());
            if(!var)
            {
                onRunError(order,"数组索引查找失败");
                return false;//暂停执行脚本
            }
        }
        else//变量未初始化
        {
            onRunError(order,"无效索引");
            return false;//暂停执行脚本
        }

        out->ref(var,true);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取数组元素值
    bool AKHandleArr::arrGet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        AKVar *var=NULL;
        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            var=arr->get(index->toUint());
            if(var)
            {
                out->setValue(var);
            }
            else
            {
                onRunError(order,"数组索引查找失败");
                return false;//暂停执行脚本
            }
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=arr->get(index->toString());
            if(var)
            {
                out->setValue(var);
            }
            else
            {
                onRunError(order,"数组索引查找失败");
                return false;//暂停执行脚本
            }
        }
        else//变量未初始化
        {
            onRunError(order,"无效索引");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符串分割为数组
    bool AKHandleArr::strSplit(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        arr->clear();

        //分割成数组
        arr->fromSplitString(
                order->getParam(2)->toString(),//源字符串
                order->getParam(3)->toString()//分割字符串
        );

        out->setValue((void *)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字符串分割为数组
    bool AKHandleArr::strJoin(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        container::AKArr *arr=(container::AKArr *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkArr(arr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }

        //连接符
        std::string &joinStr=order->getParam(2)->toStdString();

        std::string str;
        uint32 c=arr->size();
        if(c>0)
        {
            AKVar *var=arr->get((uint32)0);
            str+=var->toStdString();
        }
        for(uint32 i=1;i<c;++i)
        {
            AKVar *var=arr->get(i);
            str+=joinStr;
            str+=var->toStdString();
        }

        out->setValue(str.c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
