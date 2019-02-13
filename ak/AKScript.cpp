#include "ak.h"
#include "AKScript.h"
#include "AKXmlMgr.h"
#include "AKGlobal.h"
////////////////////////////////////////////////////////////////////
#include "AKHandleOperator.h"
#include "AKHandleMath.h"
#include "AKHandleBuf.h"
#include "AKHandleArr.h"
#include "AKHandleDict.h"
#include "AKHandleString.h"
#include "AKHandleTime.h"
#include "AKHandleCommon.h"
#include "AKHandleProcess.h"
#include "AKHandleXml.h"
#include "AKHandleNet.h"
#include "AKHandleMysql.h"
#include "AKHandleFile.h"
////////////////////////////////////////////////////////////////////

namespace ak
{
    AKScript::AKScript(
            AKSystem *system,//系统对象
            AKCore *core,//核对象
            AKProcess *process,//进程对象
            AKScript *parentScript//父级脚本
    )
            :mSystem(system),
             mCore(core),
             mProcess(process),
             mParentScript(parentScript),
             mCallType(0),
             mUrl(""),
             mbRun(false),
             mCodeTxt(""),
             mMidCodeTxt(""),
             mbIsNeedReset(false),
             mOrderPos(0),
             mOrderStatus(0),
             mIsTry(false),
             mIsLoadMidCode(false),
             mDataBuf(core),
             mDataPosBuf(core),
             mRtVar(core),
             mIsFullRun(true)
    {
        regHandler(AKHandleProcess::create(),"AKHandleProcess");//内置进程指令处理器
        regHandler(AKHandleXml::create(),"AKHandleXml");//内置xml指令处理器
        regHandler(AKHandleOperator::create(),"AKHandleOperator");//内置运算指令处理器
        regHandler(AKHandleMath::create(),"AKHandleMath");//内置数学指令处理器
        regHandler(AKHandleString::create(),"AKHandleString");//内置字符串指令处理器
        regHandler(AKHandleBuf::create(),"AKHandleBuf");//内置buf指令处理器
        regHandler(AKHandleArr::create(),"AKHandleArr");//内置数组指令处理器
        regHandler(AKHandleDict::create(),"AKHandleDict");//内置字典指令处理器
        regHandler(AKHandleTime::create(),"AKHandleTime");//内置时间指令处理器
        regHandler(AKHandleCommon::create(),"AKHandleCommon");//内置公用指令处理器
        regHandler(AKHandleNet::create(),"AKHandleNet");//内置net指令处理器
        regHandler(AKHandleMysql::create(),"AKHandleMysql");//内置mysql指令处理器
        regHandler(AKHandleFile::create(),"AKHandleFile");//内置文件指令处理器

        mSystem->onRegHandler(this);
    }

    AKScript::~AKScript()
    {
        {
            AKOrderArray::iterator it=mOrderArray.begin();
            while(it!=mOrderArray.end())
            {
                delete (*it);
                ++it;
            }
            mOrderArray.clear();
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

        {
            AKHandlerList::iterator it=mHandlerList.begin();
            while(it!=mHandlerList.end())
            {
                delete *it;
                ++it;
            }
            mHandlerList.clear();
        }
        {
            AKScriptMap::iterator it=mScriptFuncMap.begin();
            while(it!=mScriptFuncMap.end())
            {
                delete it->second;
                ++it;
            }
            mScriptFuncMap.clear();
            mScriptFuncNameMap.clear();
        }
    }

    std::string AKScript::help()
    {
        std::string str;
        AKStringStringMap::iterator it=mOrderRemarkMap.begin();
        while(it!=mOrderRemarkMap.end())
        {
            str+=it->first;
            str+="\t\t\t\t\t\t\t\t";
            str+=it->second;
            str+="\r\n";
            ++it;
        }

        return str;
    }

    std::string AKScript::helpForNotePad()
    {
        std::string str;
        str+="<?xml version=\"1.0\"?>\r\n";
        str+="<NotepadPlus>\r\n";
        str+="<AutoComplete language=\"C\">\r\n";
        str+="<Environment ignoreCase=\"no\" startFunc=\"(\" stopFunc=\")\" paramSeparator=\",\" terminal=\";\" />\r\n";

        AKStringStringMap::iterator it=mOrderRemarkMap.begin();
        while(it!=mOrderRemarkMap.end())
        {
            str+="<KeyWord name=\"";
            str+=it->first;
            str+="\" func=\"yes\">\r\n";

            str+="<Overload retVal=\"\" >\r\n";
            str+="<Param name=\">>>>    ";
            std::string t=it->second;
            AKCommon::strReplace(t,"\"","&quot;");
            str+=t;
            str+="    <<<<\" />\r\n";
            str+="</Overload>\r\n";
            str+="</KeyWord>\r\n";

            ++it;
        }

        str+="</AutoComplete>\r\n";
        str+="</NotepadPlus>\r\n";

        return str;
    }

    AKScript *AKScript::getTopParentScript()
    {
        if(!mParentScript)return NULL;
        AKScript *parent=mParentScript;
        while(parent->mParentScript)
        {
            parent=parent->mParentScript;
        }
        return parent;
    }

    AKScript *AKScript::getParentScript()
    {
        return mParentScript;
    }

    AKSystem *AKScript::getSystem()
    {
        return mSystem;
    }

    AKCore *AKScript::getCore()
    {
        return mCore;
    }

    AKProcess *AKScript::getProcess()
    {
        return mProcess;
    }

    void AKScript::setUrl(const int8 *url)
    {
        mUrl=AKCommon::dirFormat(url);
        if(mUrl!="")
        {
            int32 pos=mUrl.rfind("/");
            if(pos>=0)
            {
                mDir=mUrl.substr(0,pos+1);
                mName=mUrl.substr(pos+1);
            }
            else
            {
                mDir="";
                mName=mUrl;
            }
            pos=mName.find(".aks");
            mParentName=mName.substr(0,pos+4);
        }
        mIsLoadMidCode=false;//重置中间代码

        //重置脚本状态
        reset();
    }

    const int8 *AKScript::getUrl()
    {
        return mUrl.c_str();
    }

    const int8 *AKScript::getDir()
    {
        return mDir.c_str();
    }

    const int8 *AKScript::getName()
    {
        return mName.c_str();
    }

    const int8 *AKScript::getParentName()
    {
        return mParentName.c_str();
    }

    void AKScript::startTry()
    {
        mCatchError="";
        mIsTry=true;
    }

    void AKScript::closeTry()
    {
        mIsTry=false;
    }

    bool AKScript::tryCatch(
            const int8 *error//错误信息
    )
    {
        mCatchError=error;
        if(mIsTry)//已启动异常处理机制
        {
            //查找catch()指令
            uint32 orderCount=mOrderArray.size();
            for(uint32 i=mOrderPos+1;i<orderCount;++i)
            {
                AKOrder *order=mOrderArray[i];
                std::string &orderName=order->params[0];
                if(orderName=="catch")
                {
                    mOrderPos=i;
                    getSystem()->onRunCatch(error);
                    return true;
                }
            }
        }
        mbRun=false;//结束当前脚本调用
        if(mParentScript)//存在父级调用
        {
            return mParentScript->tryCatch(error);
        }
        return false;
    }

    const int8 *AKScript::catchError()
    {
        return mCatchError.c_str();
    }

    bool AKScript::loadMidCode()
    {
        std::string path=mUrl;
        path+=".xml";

        tinyxml2::XMLDocument *xml=AKXmlMgr::getSingleton().get(path.c_str());
        if(!xml)//加载失败
        {
            mbRun=false;
            return false;
        }
        else//加载成功
        {
            const tinyxml2::XMLElement *root=xml->RootElement();
            const tinyxml2::XMLElement *varNode=root->FirstChildElement("v");
            while(varNode)
            {
                AKVar *var=new AKVar(getCore());
                const int8 *varName=varNode->Attribute("n");
                /*var->mName+=mUrl;
                var->mName+=":";
                var->mName+=varName;*/
                uint32 varTypeInt=0;
                const int8 *varType=varNode->Attribute("t");
                if(varType)
                {
                    if(strcmp(varType,"n")==0)varTypeInt=AKVar::typeNumber;
                    else if(strcmp(varType,"s")==0)varTypeInt=AKVar::typeString;
                }
                const int8 *varVal=varNode->GetText();
                if(varVal)
                {
                    switch(varTypeInt)
                    {
                        case AKVar::typeNumber:
                        {
                            var->initValue(varVal,varTypeInt);
                            var->setValue(atof(varVal));
                            break;
                        }
                        case AKVar::typeString:
                        {
                            std::string tmp=varVal;
                            std::string valString=tmp.substr(1,tmp.size()-2);
                            var->initValue(valString.c_str(),varTypeInt);
                            var->setValue(valString.c_str());
                            break;
                        }
                    }
                }
                else
                {
                    var->initValue("",varTypeInt);
                }

                mVarMap[varName]=var;
                varNode=varNode->NextSiblingElement("v");
            }

            const tinyxml2::XMLElement *orderNode=root->FirstChildElement("o");
            while(orderNode)
            {
                const int8 *orderName=orderNode->Attribute("n");
                const int8 *rowNum=orderNode->Attribute("r");
                if(!rowNum)rowNum="0";
                const tinyxml2::XMLElement *output=orderNode->FirstChildElement("o");
                if(!output)
                {
                    mbRun=false;
                    return false;
                }
                const int8 *outputVarName=output->GetText();

                AKOrder *order=new AKOrder();
                order->rowNum=rowNum;
                AKStringArray &params=order->params;
                params.push_back(orderName);
                params.push_back(outputVarName);

                const tinyxml2::XMLElement *inputNode=orderNode->FirstChildElement("i");
                while(inputNode)
                {
                    const int8 *inputVarName=inputNode->GetText();
                    params.push_back(inputVarName);
                    inputNode=inputNode->NextSiblingElement("i");
                }
                mOrderArray.push_back(order);
                orderNode=orderNode->NextSiblingElement("o");
            }
        }

        uint32 i;
        uint32 m=mOrderArray.size();
        for(i=0;i<m;++i)
        {
            bool rt=checkParams(mOrderArray[i]);
            if(!rt)
            {
                mbRun=false;
                return false;
            }
        }

        mIsLoadMidCode=true;//中间代码加载成功
        return true;
    }

    void AKScript::reset()
    {
        //清空所有指令
        mOrderArray.clear();
        mDataBuf.clear();
        mDataPosBuf.clear();
        mRtVar.clearType();

        //删除所有变量
        AKVarMap::iterator it=mVarMap.begin();
        while(it!=mVarMap.end())
        {
            delete it->second;
            ++it;
        }
        mVarMap.clear();

        //重置运行状态
        mOrderPos=0;
        mOrderStatus=0;
        mbRun=true;

        //重置所有指令处理器
        resetOrderHandler();
    }

    void AKScript::quickReset()
    {
        mRtVar.clearType();

        //重置所有变量为初始值
        AKVarMap::iterator it=mVarMap.begin();
        while(it!=mVarMap.end())
        {
            it->second->resetValue();//重置为初始值
            ++it;
        }

        //重置运行状态
        mOrderPos=0;
        mOrderStatus=0;
        mbRun=true;

        //重置所有指令处理器
        resetOrderHandler();
    }

    void AKScript::resetOrderHandler()
    {
        AKHandlerList::iterator it=mHandlerList.begin();
        while(it!=mHandlerList.end())
        {
            (*it)->reset();
            ++it;
        }
    }

    void AKScript::setMidCode(const int8 *midCode)
    {

    }

    AKVar *AKScript::getVar(const int8 *name)
    {
        AKVarMap::iterator it=mVarMap.find(name);
        if(it==mVarMap.end())return NULL;
        return it->second;
    }

    const int8 *AKScript::getVarValue(const int8 *name)
    {
        AKVar *var=getVar(name);
        if(var)return var->toString();
        return "";
    }

    uint32 AKScript::getOrderStatus()
    {
        return mOrderStatus;
    }

    void AKScript::setOrderStatus(uint32 orderStatus)
    {
        mOrderStatus=orderStatus;
    }

    //检查参数是否存在并建立实际的变量对象数组
    bool AKScript::checkParams(
            AKOrder *order//指令对象
    )
    {
        uint32 i;
        uint32 m=order->params.size();
        for(i=1;i<m;++i)
        {
            std::string &varName=order->params[i];
            AKVar *var=getVar(varName.c_str());
            if(!var)
            {
                int8 error[1024];
                sprintf(error,"%s : 变量%s不存在",getUrl(),varName.c_str());
                mOrderInitErrorMap[order->params[0]]=error;
                throw error;
                /*onRunError(error);
                return false;*/
            }
            order->varParams.push_back(var);
        }
        return true;
    }

    void AKScript::onRunError(const int8 *error)
    {
        //错误预处理
        getSystem()->onPreError(this,error);
    }

    AKScript *AKScript::getScriptFunc(
            const int8 *funcVarName//脚本函数句柄变量名(由脚本中func api创建）
    )
    {
        AKVar *scriptFuncVar=getVar(funcVarName);//查找脚本函数句柄变量
        if(scriptFuncVar&&scriptFuncVar->getType()==AKVar::typeHandle)
        {
            AKScript *scriptFunc=(AKScript *)scriptFuncVar->toHandle();
            if(checkScriptFunc(scriptFunc))//脚本函数存在
            {
                return scriptFunc;
            }
        }
        return NULL;
    }

    bool AKScript::run(uint32 executeOrderCount)
    {
        AKGlobal &global=AKGlobal::getSingleton();
        uint32 orderCount=0;
        if(!mbRun)goto COMPLETE;

        if(!mIsLoadMidCode)
        {
            bool rt=loadMidCode();//加载中间代码
            if(!rt)
            {
                int8 error[1024];
                sprintf(error,"错误：加载 %s 错误",getUrl());
                throw error;
                //onRunError(error);
                goto COMPLETE;
            }
        }

        orderCount=mOrderArray.size();
        for(uint32 i=0;mIsFullRun||i<executeOrderCount||executeOrderCount==0;++i)
        {
            if((i%100)==0)//每执行若干次指令，检查一下是否超时
            {
                double dt=AKCommon::time_milli()-getProcess()->mRunBeginTime;
                if(dt>=10)
                {
                    break;
                }
            }
            if(getProcess()->mIsNoBreakPoint==false&&global.mBreakPointMap.size()>0)//存在断点
            {
                AKRowIDMap *rowIDMap=global.getBreakPointRowIDMap(getUrl());
                if(rowIDMap)
                {
                    if(rowIDMap->find(mOrderPos)!=rowIDMap->end())//到达断点//AK_DEBUG
                    {
                        goto RUNNING;//脚本执行未结束
                    }
                }
            }
            if(mOrderPos<orderCount)
            {
                AKOrder *order=mOrderArray[mOrderPos];
                AKScriptFunc &orderFunction=order->func;
                std::string &orderName=order->params[0];
                if(orderFunction==NULL)
                {
                    AKOrderMap::iterator it=mOrderDefineMap.find(orderName);//根据指令名查找内置函数api
                    if(it!=mOrderDefineMap.end())
                    {
                        orderFunction=it->second;
                    }
                    else
                    {
                        //重置order
                        order->apiName="";
                        order->scriptFuncVar=NULL;

                        //根据脚本API名，创建或找到脚本函数
                        AKStringStringMap &apiMap=AKGlobal::getSingleton().mAPIMap;
                        AKStringStringMap::iterator itAPI=apiMap.find(orderName);
                        if(itAPI!=apiMap.end())//script api存在
                        {
                            order->apiName=orderName;
                            it=mOrderDefineMap.find("exec@2");//绑定内置函数api
                            orderFunction=it->second;
                        }
                        else
                        {
                            //根据指令名查找脚本函数（由脚本中func api创建）
                            AKVar *scriptFuncVar=getVar(orderName.c_str());//查找脚本函数句柄变量
                            if(scriptFuncVar&&scriptFuncVar->getType()==AKVar::typeHandle||//脚本函数句柄存在
                               scriptFuncVar&&scriptFuncVar->getType()==AKVar::typeString//脚本函数url字符串存在
                                    )
                            {
                                order->scriptFuncVar=scriptFuncVar;
                                it=mOrderDefineMap.find("exec@2");//绑定内置函数api
                                orderFunction=it->second;
                            }
                            else
                            {
                                int8 tmp[1024];
                                sprintf(tmp,"%s api %s() 未注册",getUrl(),orderName.c_str());
                                onRunError(tmp);
                                goto RUNNING;//脚本执行未结束
                            }
                        }
                    }
                }
                try
                {
                    bool bContinue=orderFunction(this,order);
                    if(!bContinue)//暂停执行脚本
                    {
                        break;
                    }
                }
                catch(const int8 *error)
                {
                    //输出错误信息
                    int8 tmp[2048];
                    sprintf(tmp,"%s api %s() 异常，%s",getUrl(),orderName.c_str(),error);

                    //输出指令码
                    AKStringArray &params=order->params;
                    int8 tmp2[1024];
                    sprintf(tmp2,"    %s=%s(",params[1].c_str(),params[0].c_str());
                    uint32 c=params.size();
                    for(uint32 i=2;i<c;++i)
                    {
                        if(i>2)strcat(tmp2,",");
                        strcat(tmp2,params[i].c_str());
                    }
                    strcat(tmp2,");");
                    strcat(tmp,tmp2);

                    onRunError(tmp);
                    goto RUNNING;//脚本执行未结束
                }
            }
            else
            {
                mbRun=false;
                goto COMPLETE;//脚本执行结束
            }
        }

        RUNNING://脚本执行未结束
        return true;
        COMPLETE://脚本执行结束
        return false;
    }

    void AKScript::nextOrder()
    {
        mOrderStatus=0;//正在执行的指令状态清0
        ++mOrderPos;
    }

    void AKScript::regOrderFunction(
            const int8 *name,
            AKScriptFunc orderFunc,
            const int8 *remark
    )
    {
        mOrderDefineMap[name]=orderFunc;
        if(strcmp(remark,"")!=0)mOrderRemarkMap[name]=remark;
    }

    const int8 *AKScript::getOrderFunctionRemark(
            const int8 *name//函数名
    )
    {
        AKStringStringMap::iterator it=mOrderRemarkMap.find(name);
        if(it==mOrderRemarkMap.end())return "";
        return it->second.c_str();
    }

    void AKScript::regHandler(AKHandler *handler,const int8 *name)
    {
        handler->init(this);
        handler->mName=name;
        mHandlerList.push_back(handler);
        if(name)
        {
            mHandlerMap[name]=handler;
        }
    }

    AKHandler *AKScript::getHandler(const int8 *name)
    {
        AKHandlerMap::iterator it=mHandlerMap.find(name);
        if(it==mHandlerMap.end())return NULL;
        return it->second;
    }

    void AKScript::setOrderPos(uint32 pos)
    {
        mOrderPos=pos;
    }

    uint32 AKScript::getOrderCount()
    {
        return mOrderArray.size();
    }

    AKOrder *AKScript::getCurOrder()
    {
        if(mOrderPos>=mOrderArray.size())
        {
            return NULL;
        }

        return mOrderArray[mOrderPos];
    }

    //停止执行脚本
    void AKScript::stop()
    {
        mbRun=false;
        mOrderPos=mOrderArray.size();
    }

    //是否运行中
    bool AKScript::isRunning()
    {
        return mbRun;
    }

    //获取调用脚本堆栈数组
    void AKScript::getCallStackArr(
            std::vector<AKScript *> &callStackArr//调用脚本堆栈数组
    )
    {
        AKScript *curScript=this;
        while(curScript)
        {
            callStackArr.push_back(curScript);
            curScript=curScript->getParentScript();
        }
    }

    AKScript *AKScript::createScriptFunc(const int8 *scriptUrl)
    {
        AKStringScriptMap::iterator it=mScriptFuncNameMap.find(scriptUrl);
        if(it!=mScriptFuncNameMap.end())
        {
            return it->second;//同一个脚本路径只存在一个脚本函数对象
        }

        AKScript *scriptFunc=new AKScript(getSystem(),getCore(),getProcess(),this);
        scriptFunc->setUrl(scriptUrl);
        mScriptFuncMap[scriptFunc]=scriptFunc;
        mScriptFuncNameMap[scriptUrl]=scriptFunc;
        return scriptFunc;
    }

    bool AKScript::checkScriptFuncCanExec(AKScript *scriptFunc)
    {
        //递归检查当前脚本与其所有父
        AKScript *curScriptFunc=this;//当前脚本
        while(curScriptFunc)
        {
            if(scriptFunc==curScriptFunc)return false;//同一个脚本函数实例被递归调用
            curScriptFunc=curScriptFunc->getParentScript();
        }
        return true;
    }

    bool AKScript::checkScriptFunc(AKScript *scriptFunc)
    {
        if(mScriptFuncMap.find(scriptFunc)!=mScriptFuncMap.end())return true;
        AKScript *parent=getParentScript();
        if(!parent)return false;
        return parent->checkScriptFunc(scriptFunc);
    }

}
