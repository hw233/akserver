#include "ak.h"
#include "AKScript.h"
#include "AKHandleDict.h"
#include "AKHandleXml.h"
#include "AKGlobal.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleDict::AKHandleDict()
            :mParent(NULL)
    {
    }

    AKHandleDict::~AKHandleDict()
    {
        reset();
    }

    container::AKDictIterator *AKHandleDict::createDictIt()
    {
        container::AKDictIterator *it=new ak::container::AKDictIterator();
        mDictIteratorMap[it]=it;
        return it;
    }

    bool AKHandleDict::checkDictIt(container::AKDictIterator *it)
    {
        if(mDictIteratorMap.find(it)!=mDictIteratorMap.end())return true;
        if(!mParent)
        {
            AKScript *parentScript=getScript()->getParentScript();
            if(!parentScript)
            {
                return false;
            }
            mParent=(AKHandleDict *)parentScript->getHandler("AKHandleDict");
        }
        return mParent->checkDictIt(it);
    }

    container::AKDict *AKHandleDict::createDictFromXml(//根据xml创建字典
            tinyxml2::XMLDocument *xml,
            const int8 *keyName,//作为索引的键名
            const int8 *rowName//行节点名
    )
    {
        container::AKDict *dict=getScript()->getCore()->createDict();
        tinyxml2::XMLElement *root=xml->RootElement();

        //首行
        tinyxml2::XMLElement *row=NULL;
        if(rowName)row=root->FirstChildElement(rowName);
        else row=root->FirstChildElement();

        while(row)
        {
            //查找键
            std::string keyVal;
            tinyxml2::XMLElement *col=row->FirstChildElement();//列
            while(col)
            {
                const int8 *colName=col->Value();//列名
                if(colName&&strcmp(colName,keyName)==0)//找到键
                {
                    const int8 *val=col->GetText();
                    if(val)
                    {
                        keyVal=val;
                    }
                    break;
                }
                col=col->NextSiblingElement();
            }

            if(keyVal!="")//找到键
            {
                //创建字典记录
                container::AKDict *rsDict=getScript()->getCore()->createDict();//字典记录

                //填写字典记录
                col=row->FirstChildElement();//列
                while(col)
                {
                    const int8 *colName=col->Value();//列名
                    const int8 *val=col->GetText();
                    if(!val)val="";
                    if(colName)
                    {
                        rsDict->set(colName,val);
                    }
                    col=col->NextSiblingElement();
                }

                //保存
                dict->set((const int8 *)keyVal.c_str(),(void *)rsDict);
            }

            //下一行
            if(rowName)row=row->NextSiblingElement(rowName);
            else row=row->NextSiblingElement();
        }
        return dict;
    }

    void AKHandleDict::reset()
    {
        {
            AKDictIteratorMap::iterator it=mDictIteratorMap.begin();
            while(it!=mDictIteratorMap.end())
            {
                delete it->second;
                ++it;
            }
            mDictIteratorMap.clear();
        }

    }

    void AKHandleDict::init(AKScript *script)
    {
        AKHandler::init(script);

        //字典基本api
        AKQuickBindScriptFunc("dictDestroy",&AKHandleDict::dictDestroy,"dictDestroy(arr);//销毁字典");
        AKQuickBindScriptFunc("dict",&AKHandleDict::dictCreate,"dict=dict([key1,val1,key2,val2,...]);//创建字典");
        AKQuickBindScriptFunc("dictCreate",&AKHandleDict::dictCreate,"//同dict()，已淘汰");
        AKQuickBindScriptFunc("gDictCreate",&AKHandleDict::dictCreate,"//同dict()，已淘汰");
        AKQuickBindScriptFunc("pDictCreate",&AKHandleDict::dictCreate,"//同dict()，已淘汰");
        AKQuickBindScriptFunc("dictSize",&AKHandleDict::dictSize,"size=dictSize(dict);//获取字典尺寸");
        AKQuickBindScriptFunc("dictClear",&AKHandleDict::dictClear,"dictClear(dict);//清空字典");
        AKQuickBindScriptFunc("dictSet",&AKHandleDict::dictSet,"dictSet(dict,key,val);//设置字典元素值");
        AKQuickBindScriptFunc("dictGet",&AKHandleDict::dictGet,"val=dictGet(dict,key1[,key2,...]);//获取字典元素值");
        AKQuickBindScriptFunc("dictExist",&AKHandleDict::dictExist,"isExist=dictExist(dict,key);//查询字典元素是否存在");
        AKQuickBindScriptFunc("dictRemove",&AKHandleDict::dictRemove,"dictRemove(dict,key);//删除字典元素");
        AKQuickBindScriptFunc("dictGetRef",&AKHandleDict::dictGetRef,"ref=dictGetRef(dict,key1[,key2,...]);//获取字典元素引用");
        AKQuickBindScriptFunc("isDict",&AKHandleDict::isDict,"isDict=isDict(var);//判断是否为字典");

        //字典迭代器api
        AKQuickBindScriptFunc("dictIt",&AKHandleDict::dictIt,"it=dictIt();//创建字典迭代器");
        AKQuickBindScriptFunc("dictItBegin",&AKHandleDict::dictBegin,"dictItBegin(dict,it);//返回字典开始迭代器");
        AKQuickBindScriptFunc("dictItEnd",&AKHandleDict::dictEnd,"dictItEnd(dict,it);//返回字典结束迭代器");
        AKQuickBindScriptFunc("dictItNext",&AKHandleDict::dictNext,"dictItNext(it);//字典迭代器下移");
        AKQuickBindScriptFunc("dictItPre",&AKHandleDict::dictPre,"dictItPre(it);//字典迭代器上移");
        AKQuickBindScriptFunc("dictItEmpty",&AKHandleDict::dictItEmpty,"dictItEmpty(it);//字典迭代器是否为空");
        AKQuickBindScriptFunc("dictItKey",&AKHandleDict::dictItKey,"key=dictItKey(it);//字典迭代器取键名");
        AKQuickBindScriptFunc("dictItVal",&AKHandleDict::dictItVal,"val=dictItVal(it);//字典迭代器取值");
        AKQuickBindScriptFunc("dictBegin",&AKHandleDict::dictBegin,"//同dictItBegin");
        AKQuickBindScriptFunc("dictEnd",&AKHandleDict::dictEnd,"//同dictItEnd");
        AKQuickBindScriptFunc("dictNext",&AKHandleDict::dictNext,"//同dictItNext");
        AKQuickBindScriptFunc("dictPre",&AKHandleDict::dictPre,"//同dictItPre");

        //xml转字典api
        AKQuickBindScriptFunc("gDictCreateFromXml",&AKHandleDict::dictCreateFromXml,"dict=gDictCreateFromXml(xml,keyName[,rowName]);//从xml创建字典");
        AKQuickBindScriptFunc("pDictCreateFromXml",&AKHandleDict::dictCreateFromXml,"dict=pDictCreateFromXml(xml,keyName[,rowName]);//从xml创建字典");
        AKQuickBindScriptFunc("dictCreateFromXml",&AKHandleDict::dictCreateFromXml,"dict=dictCreateFromXml(xml,keyName[,rowName]);//从xml创建字典");
    }

    //判断是否为字典
    bool AKHandleDict::isDict(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        bool isDict=false;
        if(order->getParam(1)->getType()==AKVar::typeHandle)
        {
            container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
            if(script->getCore()->checkDict(dict))isDict=true;
        }

        if(isDict)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从xml创建字典
    bool AKHandleDict::dictCreateFromXml(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        tinyxml2::XMLDocument *xml=(tinyxml2::XMLDocument *)order->getParam(1)->toHandle();
        AKHandleXml *xmlHandler=dynamic_cast<AKHandleXml *>(script->getHandler("AKHandleXml"));
        if(!xmlHandler->checkXml(xml))
        {
            onRunError(order,"无效xml句柄");
            return false;//暂停执行脚本
        }

        tinyxml2::XMLElement *root=xml->RootElement();
        if(!root)
        {
            onRunError(order,"获取根节点错误");
            return false;//暂停执行脚本
        }

        const int8 *keyName=order->getParam(2)->toString();//键名
        const int8 *rowName=NULL;
        if(order->getParam(3))rowName=order->getParam(3)->toString();
        container::AKDict *dict=createDictFromXml(xml,keyName,rowName);

        out->setValue((void *)dict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典迭代器取键名
    bool AKHandleDict::dictItKey(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(1)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        container::AKDict *dict=it->getDict();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        if(it->keyString())
        {
            out->setType(AKVar::typeString);
            out->setValue(it->keyString());
        }
        else
        {
            out->setType(AKVar::typeNumber);
            out->setValue(it->keyUint32());
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典迭代器取值
    bool AKHandleDict::dictItVal(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(1)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        container::AKDict *dict=it->getDict();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        AKVar *var=it->val();
        if(var)
        {
            out->setValue(var);
        }
        else
        {
            out->setType(AKVar::typeString);
            out->setValue("");
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典迭代器是否为空
    bool AKHandleDict::dictItEmpty(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(1)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        if(*it)//迭代器非空
        {
            out->setValue((double)0);
        }
        else
        {
            out->setValue((double)1);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典迭代器上移
    bool AKHandleDict::dictPre(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(1)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        --(*it);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典迭代器下移
    bool AKHandleDict::dictNext(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(1)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        ++(*it);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //返回字典结束迭代器
    bool AKHandleDict::dictEnd(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(2)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        *it=dict->end();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //返回字典开始迭代器
    bool AKHandleDict::dictBegin(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        container::AKDictIterator *it=(container::AKDictIterator *)order->getParam(2)->toHandle();
        if(!checkDictIt(it))
        {
            onRunError(order,"无效字典迭代器");
            return false;//暂停执行脚本
        }

        *it=dict->begin();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建字典迭代器
    bool AKHandleDict::dictIt(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKDictIterator *it=createDictIt();

        out->setValue((void *)it);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //销毁字典
    bool AKHandleDict::dictDestroy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        script->getCore()->destroyRef(dict);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建字典
    bool AKHandleDict::dictCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKDict *dict=getScript()->getCore()->createDict();

        //添加字典元素
        for(uint32 i=1;order->getParam(i);i+=2)
        {
            AKVar *index=order->getParam(i);
            AKVar *var=order->getParam(i+1);
            if(!var)//只有key，没有val
            {
                onRunError(order,"创建字典时的键值列表不对应");
                return false;//暂停执行脚本
            }
            if(index->getType()==AKVar::typeNumber)
            {
                dict->set(index->toUint(),var);
            }
            else if(index->getType()==AKVar::typeString)
            {
                dict->set(index->toString(),var);
            }
            else//非法变量
            {
                onRunError(order,"无效字典索引");
                return false;//暂停执行脚本
            }
        }

        out->setValue((void *)dict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //清空字典
    bool AKHandleDict::dictClear(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        dict->clear();
        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字典尺寸
    bool AKHandleDict::dictSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        out->setValue((double)dict->size());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //查询字典元素是否存在
    bool AKHandleDict::dictExist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        uint32 rt=0;
        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            if(dict->get(index->toUint()))rt=1;
        }
        else if(index->getType()==AKVar::typeString)
        {
            if(dict->get(index->toString()))rt=1;
        }
        else//非法变量
        {
            onRunError(order,"无效字典索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除字典元素
    bool AKHandleDict::dictRemove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            dict->remove(index->toUint());
        }
        else if(index->getType()==AKVar::typeString)
        {
            dict->remove(index->toString());
        }
        else//非法变量
        {
            onRunError(order,"无效字典索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置字典元素值
    bool AKHandleDict::dictSet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        AKVar *index=order->getParam(2);
        if(index->getType()==AKVar::typeNumber)
        {
            dict->set(index->toUint(),order->getParam(3));
        }
        else if(index->getType()==AKVar::typeString)
        {
            dict->set(index->toString(),order->getParam(3));
        }
        else//非法变量
        {
            onRunError(order,"无效字典索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字典元素值
    bool AKHandleDict::dictGet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();

        AKVar *var=NULL;
        AKVar *index=NULL;
        for(uint32 i=2;order->getParam(i);++i)
        {
            if(!dict)
            {
                dict=(container::AKDict *)var->toHandle();
            }
            if(!script->getCore()->checkDict(dict))
            {
                onRunError(order,"无效字典");
                return false;//暂停执行脚本
            }

            index=order->getParam(i);
            if(index->getType()==AKVar::typeNumber)
            {
                var=dict->get(index->toUint());
            }
            else if(index->getType()==AKVar::typeString)
            {
                var=dict->get(index->toString());
            }
            else//变量未初始化
            {
                onRunError(order,"无效字典索引");
                return false;//暂停执行脚本
            }

            if(!var)
            {
                onRunError(order,"字典索引查找失败");
                return false;//暂停执行脚本
            }

            dict=NULL;//换下一个字典
        }

        out->setValue(var);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字典元素引用
    bool AKHandleDict::dictGetRef(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();

        AKVar *var=NULL;
        AKVar *index=NULL;
        for(uint32 i=2;order->getParam(i);++i)
        {
            if(!dict)
            {
                dict=(container::AKDict *)var->toHandle();
            }
            if(!script->getCore()->checkDict(dict))
            {
                onRunError(order,"无效字典");
                return false;//暂停执行脚本
            }

            index=order->getParam(i);
            if(index->getType()==AKVar::typeNumber)
            {
                var=dict->get(index->toUint());
            }
            else if(index->getType()==AKVar::typeString)
            {
                var=dict->get(index->toString());
            }
            else//变量未初始化
            {
                onRunError(order,"无效字典索引");
                return false;//暂停执行脚本
            }

            if(!var)
            {
                onRunError(order,"字典索引查找失败");
                return false;//暂停执行脚本
            }

            dict=NULL;//换下一个字典
        }

        out->ref(var,true);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
