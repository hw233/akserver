#include "ak.h"
#include "AKHandleXml.h"
#include "AKJson.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleXml::AKHandleXml()
            :mParent(NULL)
    {
    }

    AKHandleXml::~AKHandleXml()
    {
        reset();
    }

    tinyxml2::XMLDocument *AKHandleXml::createXml()
    {
        AKHandleXml *handler=(AKHandleXml *)getTopParent();
        tinyxml2::XMLDocument *xml=new tinyxml2::XMLDocument();
        handler->mXmlMap[xml]=xml;
        return xml;
    }

    bool AKHandleXml::checkXml(tinyxml2::XMLDocument *xml)
    {
        AKHandleXml *handler=(AKHandleXml *)getTopParent();
        if(handler->mXmlMap.find(xml)!=handler->mXmlMap.end())return true;
        return false;
    }

    void AKHandleXml::destroyXml(tinyxml2::XMLDocument *xml)
    {
        AKHandleXml *handler=(AKHandleXml *)getTopParent();
        AKXmlMap::iterator it=handler->mXmlMap.find(xml);
        if(it==handler->mXmlMap.end())return;

        delete it->second;
        handler->mXmlMap.erase(it);
    }

    void AKHandleXml::reset()
    {
        AKXmlMap::iterator it=mXmlMap.begin();
        while(it!=mXmlMap.end())
        {
            delete it->second;
            ++it;
        }
        mXmlMap.clear();
    }

    void AKHandleXml::init(AKScript *script)
    {
        AKHandler::init(script);

        //xml
        AKQuickBindScriptFunc("xmlCreateFromFile",&AKHandleXml::xmlCreateFromFile,"xml=xmlCreateFromFile(xmlFilePath);//从文件创建xml");
        AKQuickBindScriptFunc("xmlCreate",&AKHandleXml::xmlCreate,"xml=xmlCreate(xmlStr);//创建xml");
        AKQuickBindScriptFunc("xmlDestroy",&AKHandleXml::xmlDestroy,"xmlDestroy(xmlStr);//销毁xml");
        AKQuickBindScriptFunc("xmlGet",&AKHandleXml::xmlGet,"xml=xmlGet(xml,xmlNodeUrl);//获取xml内容，xmlNodeUrl为xml节点路径");
        AKQuickBindScriptFunc("xmlGetSize",&AKHandleXml::xmlGetSize,"count=xmlGetSize(xml,xmlNodeUrl);//获取节点路径的节点数量，xmlNodeUrl为xml节点路径");

        //json
        AKQuickBindScriptFunc("json_encode",&AKHandleXml::json_encode,"jsonStr=json_encode(obj);//对象转json字符串");
        AKQuickBindScriptFunc("json_decode",&AKHandleXml::json_decode,"obj=json_decode(jsonStr);//json字符串转对象");
        AKQuickBindScriptFunc("json",&AKHandleXml::json_decode,"obj=json(jsonStr);//同json_decode");
    }

    //json字符串转对象
    bool AKHandleXml::json_decode(AKScript *script, AKOrder *order)
    {
        if (!checkParams(1, script, order))return false;
        AKVar *out = order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        AKVar *jsonStrVar = order->getParam(1);
        bool rt=AKJson::decode(script->getCore(), out, jsonStrVar->toString(), jsonStrVar->toStringSize());
        if (!rt)
        {
            onRunError(order, "json decode error");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //对象转json字符串
    bool AKHandleXml::json_encode(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKVar *var=order->getParam(1);
        std::string jsonStr;
        AKCommon::json_encode(script->getCore(),var,jsonStr);

        out->setValue(jsonStr.c_str());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从文件创建xml
    bool AKHandleXml::xmlCreateFromFile(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *xmlFilePath=order->getParam(1)->toString();

        tinyxml2::XMLDocument *xml=createXml();
        xml->LoadFile(xmlFilePath);
        if(xml->Error())
        {
            onRunError(order,"无效xml内容");
            return false;//暂停执行脚本
        }

        out->setValue((void *)xml);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建xml
    bool AKHandleXml::xmlCreate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *xmlStr=order->getParam(1)->toString();

        tinyxml2::XMLDocument *xml=createXml();
        xml->Parse(xmlStr);
        if(xml->Error())
        {
            onRunError(order,"无效xml内容");
            return false;//暂停执行脚本
        }

        out->setValue((void *)xml);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    bool AKHandleXml::xmlDestroy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        tinyxml2::XMLDocument *xml=(tinyxml2::XMLDocument *)order->getParam(1)->toHandle();
        destroyXml(xml);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取xml内容，xmlNodeUrl为xml节点路径
    /*
    <root>
    <obj other="1">
    <id>1</id>
    <num>100</num>
    </obj>
    <obj other="2">
    <id>2</id>
    <num>200</num>
    </obj>
    </root>


    xml=xmlCreate("...");
    xmlGet(xml,"obj/@other");
    xmlGet(xml,"obj/id");
    xmlGet(xml,"obj:1/id");
    */
    bool AKHandleXml::xmlGet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        tinyxml2::XMLDocument *xml=(tinyxml2::XMLDocument *)order->getParam(1)->toHandle();
        if(!checkXml(xml))
        {
            onRunError(order,"无效xml句柄");
            return false;//暂停执行脚本
        }

        tinyxml2::XMLElement *node=xml->RootElement();
        if(!node)
        {
            onRunError(order,"获取根节点错误");
            return false;//暂停执行脚本
        }
        const int8 *rtVal=NULL;

        const int8 *xmlNodeUrl=order->getParam(2)->toString();//xml节点路径
        container::AKArr tmp(script->getCore());
        container::AKArr arr(script->getCore());
        arr.fromSplitString(xmlNodeUrl,"/");
        uint32 i=0;
        uint32 m=arr.size();
        while(i<m)
        {
            AKVar *var=arr.get(i);
            const int8 *val=var->toString();
            if(val[0]=='@')//节点属性名
            {
                const int8 *propertyName=&val[1];
                if(i!=(m-1))
                {
                    std::string error="属性名 ";
                    error+=propertyName;
                    error+=" 应该是路径中最后一个元素";
                    onRunError(order,error.c_str());
                    return false;//暂停执行脚本
                }
                rtVal=node->Attribute(propertyName);
                if(!rtVal)
                {
                    std::string error="属性名 ";
                    error+=propertyName;
                    error+=" 不存在";
                    onRunError(order,error.c_str());
                    return false;//暂停执行脚本
                }
                node=NULL;
            }
            else
            {
                tmp.clear();
                tmp.fromSplitString(val,":");
                if(tmp.size()==1)
                {
                    node=node->FirstChildElement(val);
                    if(!node)
                    {
                        std::string error="节点名 ";
                        error+=val;
                        error+=" 不存在";
                        onRunError(order,error.c_str());
                        return false;//暂停执行脚本
                    }
                }
                else if(tmp.size()==2)
                {
                    const int8 *curVal=tmp.get((uint32)0)->toString();
                    uint32 index=tmp.get((uint32)1)->toUint();
                    uint32 i=0;
                    while(i<=index)
                    {
                        if(i==0)
                        {
                            node=node->FirstChildElement(curVal);
                        }
                        else
                        {
                            node=node->NextSiblingElement(curVal);
                        }
                        if(!node)break;
                        ++i;
                    }
                    if(!node)
                    {
                        std::string error="节点名 ";
                        error+=val;
                        error+=" 不存在";
                        onRunError(order,error.c_str());
                        return false;//暂停执行脚本
                    }
                }
                else
                {
                    std::string error="错误xml路径描述 ";
                    error+=val;
                    onRunError(order,error.c_str());
                    return false;//暂停执行脚本
                }
            }
            ++i;
        }

        if(node)
        {
            rtVal=node->GetText();
            if(!rtVal)
            {
                rtVal="";
            }
        }

        out->setValue((const int8 *)rtVal);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    bool AKHandleXml::xmlGetSize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        tinyxml2::XMLDocument *xml=(tinyxml2::XMLDocument *)order->getParam(1)->toHandle();
        if(!checkXml(xml))
        {
            onRunError(order,"无效xml句柄");
            return false;//暂停执行脚本
        }

        tinyxml2::XMLElement *node=xml->RootElement();
        if(!node)
        {
            onRunError(order,"获取根节点错误");
            return false;//暂停执行脚本
        }

        const int8 *xmlNodeUrl=order->getParam(2)->toString();//xml节点路径
        container::AKArr tmp(script->getCore());
        container::AKArr arr(script->getCore());
        arr.fromSplitString(xmlNodeUrl,"/");
        uint32 i=0;
        uint32 m=arr.size();
        while(i<m)
        {
            AKVar *var=arr.get(i);
            const int8 *val=var->toString();
            if(val[0]=='@')//节点属性名
            {
                std::string error="错误xml路径描述 ";
                error+=val;
                onRunError(order,error.c_str());
                return false;//暂停执行脚本
            }
            else
            {
                tmp.clear();
                tmp.fromSplitString(val,":");
                if(tmp.size()==1)
                {
                    node=node->FirstChildElement(val);
                    if(!node)break;
                }
                else if(tmp.size()==2)
                {
                    if(i==(m-1))
                    {
                        std::string error="错误xml路径描述 ";
                        error+=val;
                        onRunError(order,error.c_str());
                        return false;//暂停执行脚本
                    }
                    const int8 *curVal=tmp.get((uint32)0)->toString();
                    uint32 index=tmp.get((uint32)1)->toUint();
                    uint32 i=0;
                    while(i<=index)
                    {
                        if(i==0)
                        {
                            node=node->FirstChildElement(curVal);
                        }
                        else
                        {
                            node=node->NextSiblingElement(curVal);
                        }
                        if(!node)break;
                        ++i;
                    }
                    if(!node)
                    {
                        std::string error="节点名 ";
                        error+=val;
                        error+=" 不存在";
                        onRunError(order,error.c_str());
                        return false;//暂停执行脚本
                    }
                }
                else
                {
                    std::string error="错误xml路径描述 ";
                    error+=val;
                    onRunError(order,error.c_str());
                    return false;//暂停执行脚本
                }
            }
            ++i;
        }

        uint32 nodeCount=0;
        if(node)
        {
            nodeCount=1;
            while((node=node->NextSiblingElement(node->Value())))
            {
                ++nodeCount;
            }
        }

        out->setValue((double)nodeCount);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
