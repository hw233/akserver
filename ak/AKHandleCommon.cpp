#include "ak.h"
#include "AKScript.h"
#include "AKHandleCommon.h"
#include "AKHandleBuf.h"
#include "AKGlobal.h"
#include "AKXmlMgr.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleCommon::AKHandleCommon()
    {
    }

    AKHandleCommon::~AKHandleCommon()
    {
    }

    void AKHandleCommon::init(AKScript *script)
    {
        AKHandler::init(script);

        //常用api
        AKQuickBindScriptFunc("argc",&AKHandleCommon::myargc,"argc();//获取命令行参数数量");
        AKQuickBindScriptFunc("argv",&AKHandleCommon::myargv,"var=argv(i);//获取第i个命令行参数");
        AKQuickBindScriptFunc("help",&AKHandleCommon::help,"help([\"help.txt\"]);//输出所有API帮助信息到文件");
        AKQuickBindScriptFunc("trace",&AKHandleCommon::trace,"trace(\"xxx\");//跟踪打印信息");
        AKQuickBindScriptFunc("echo",&AKHandleCommon::echo,"echo(\"xxx\");//向标准输出流stdout输出字符串");
        AKQuickBindScriptFunc("link",&AKHandleCommon::link,"ref=link(varName);//创建调用堆栈中变量的引用");
        AKQuickBindScriptFunc("ref",&AKHandleCommon::_ref,"ref1=ref(a);ref2=ref(a.b);ref3=ref(a[i]);//创建引用");
        AKQuickBindScriptFunc("refCount",&AKHandleCommon::refCount,"refCount=refCount(arr);refCount=refCount(dict);refCount=refCount(buf);//获取容器引用计数");
        AKQuickBindScriptFunc("hotUpdate",&AKHandleCommon::hotUpdate,"hotUpdate();//脚本热更新");
        AKQuickBindScriptFunc("scriptUrl",&AKHandleCommon::scriptUrl,"scriptUrl();//获取脚本URL");
        AKQuickBindScriptFunc("scriptDir",&AKHandleCommon::scriptDir,"dir=scriptDir();//获取脚本目录");
        AKQuickBindScriptFunc("full",&AKHandleCommon::full,"url=full(\"?1.aks\");//相对路径转绝对路径");
        AKQuickBindScriptFunc("quitSystem",&AKHandleCommon::quitSystem,"quitSystem();//退出系统");
        AKQuickBindScriptFunc("autoSleep",&AKHandleCommon::autoSleep,"autoSleep(dt);//设置系统自动休眠时间（毫秒）");
        AKQuickBindScriptFunc("fullRun",&AKHandleCommon::fullRun,"fullRun(isFullRun);//设置是否全速执行当前脚本（全速执行时不会受指令执行数量限制）");
        AKQuickBindScriptFunc("api",&AKHandleCommon::api,"api(apiName,scriptUrl);//脚本函数注册为API，然后可以像内置API一样调用");
        AKQuickBindScriptFunc("platform",&AKHandleCommon::platform,"platformSign=platform();//获取平台标识，win或other");
        AKQuickBindScriptFunc("errorLog",&AKHandleCommon::errorLog,"errorLog(errorLogFilePath);//记录所有错误信息到文件（不管该错误是否被异常捕捉）");
        AKQuickBindScriptFunc("shell",&AKHandleCommon::_shell,"shell(cmdStr);//启动新进程执行命令（暂时只支持windows命令）");

        //调试工具API
        AKQuickBindScriptFunc("null",&AKHandleCommon::null,"isNull=null(var);//判断变量是否为null");
        AKQuickBindScriptFunc("var",&AKHandleCommon::var,"val=var(varName);//获取变量值（根据变量名）");
        AKQuickBindScriptFunc("varNameArr",&AKHandleCommon::varNameArr,"varNameArr=varNameArr();//获取所有变量的名字数组");
        AKQuickBindScriptFunc("getCoreRunTimeInfo",&AKHandleCommon::getCoreRunTimeInfo,"infoStr=getCoreRunTimeInfo();//获取当前核的运行时信息");
        AKQuickBindScriptFunc("getScriptMidCode",&AKHandleCommon::getScriptMidCode,"rowArr=getScriptMidCode(scriptUrl);//获取脚本中间代码（返回中间代码行数组）");

        //断点相关API
        AKQuickBindScriptFunc("addBreakPoint",&AKHandleCommon::addBreakPoint,"addBreakPoint(scriptUrl,rowID);//设置断点");
        AKQuickBindScriptFunc("isBreakPointExist",&AKHandleCommon::isBreakPointExist,"isExist=isBreakPointExist(scriptUrl,rowID);//检查断点是否存在");
        AKQuickBindScriptFunc("delBreakPoint",&AKHandleCommon::delBreakPoint,"delBreakPoint(scriptUrl,rowID);//删除断点");
        AKQuickBindScriptFunc("delAllBreakPoint",&AKHandleCommon::delAllBreakPoint,"delAllBreakPoint();//删除所有断点");
        AKQuickBindScriptFunc("pNoBreakPoint",&AKHandleCommon::pNoBreakPoint,"pNoBreakPoint();//禁止当前进程使用断点");
        AKQuickBindScriptFunc("getBreakPointDict",&AKHandleCommon::getBreakPointDict,"breakPointDict=getBreakPointDict();//获取断点字典");

        //全局变量api
        AKQuickBindScriptFunc("gSet",&AKHandleCommon::gSet,"gSet(key,val);//设置核全局变量值");
        AKQuickBindScriptFunc("gGet",&AKHandleCommon::gGet,"val=gGet(key);//获取核全局变量值");
        AKQuickBindScriptFunc("gRemove",&AKHandleCommon::gRemove,"gRemove(key);//删除核全局变量");
        AKQuickBindScriptFunc("gExist",&AKHandleCommon::gExist,"isExist=gExist(key);//全局变量是否存在");

        //通用容器api
        AKQuickBindScriptFunc("clean",&AKHandleCommon::clean,"arr=clean(arr);dict=clean(dict);buf=clean(buf);//清空缓冲、字典或数组");
        AKQuickBindScriptFunc("exist",&AKHandleCommon::exist,"isExist=exist(arr,key);isExist=exist(dict,key);//检查字典或数组的键值是否存在");
        AKQuickBindScriptFunc("push",&AKHandleCommon::push,"push(arr,val1[,val2,...]);push(dict,key1,val1[key2,val2,...]);//压入字典或数组多个值");
        AKQuickBindScriptFunc("set",&AKHandleCommon::set,"set(arr,key,val);set(dict,key,val);//设置字典或数组的值");
        AKQuickBindScriptFunc("get",&AKHandleCommon::get,"val=get(arr,key);val=get(dict,key);//获取字典或数组的值");
        AKQuickBindScriptFunc("getRef",&AKHandleCommon::getRef,"ref=getRef(arr,key);ref=getRef(dict,key);//获取字典或数组的值的引用");
        AKQuickBindScriptFunc("size",&AKHandleCommon::size,"btyes=size(dict);btyes=size(arr);btyes=size(buf);//获取字典、数组或缓冲的尺寸");
        AKQuickBindScriptFunc("clone",&AKHandleCommon::clone,"newVar=clone(var);//克隆变量或嵌套容器");
        AKQuickBindScriptFunc("remove",&AKHandleCommon::remove,"remove(dict,key);remove(arr,key);//删除字典或数组元素");
        AKQuickBindScriptFunc("inArr",&AKHandleCommon::inArr,"isInArr=inArr(arr,val);//指定的值是否存在于数组中");
    }

    //判断变量是否为null
    bool AKHandleCommon::null(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKVar *var=order->getParam(1);
        if(var->getType()==0)
        {
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //启动新进程执行命令
    bool AKHandleCommon::_shell(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *cmdStr=order->getParam(1)->toString();

#ifdef WIN32
        WinExec(cmdStr,SW_SHOW);
#else
        pid_t fpid=fork();
        if(fpid==0)//子进程中
        {
            system(cmdStr);
            exit(0);
        }
#endif

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取断点字典
    bool AKHandleCommon::getBreakPointDict(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKDict *breakPointDict=script->getCore()->createDict();
        AKBreakPointMap &breakPointMap=AKGlobal::getSingleton().mBreakPointMap;
        AKBreakPointMap::iterator it=breakPointMap.begin();
        while(it!=breakPointMap.end())
        {
            const std::string &scriptUrl=it->first;
            AKRowIDMap &rowIDMap=it->second;

            container::AKDict *rowIDDict=script->getCore()->createDict();
            breakPointDict->set(scriptUrl.c_str(),(void *)rowIDDict);

            AKRowIDMap::iterator it2=rowIDMap.begin();
            while(it2!=rowIDMap.end())
            {
                uint32 rowID=it2->first;
                rowIDDict->set(rowID,(double)rowID);
                ++it2;
            }
            ++it;
        }

        out->setValue((void *)breakPointDict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //禁止当前进程使用断点
    bool AKHandleCommon::pNoBreakPoint(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        script->getProcess()->mIsNoBreakPoint=true;

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取脚本中间代码（返回中间代码行数组）
    bool AKHandleCommon::getScriptMidCode(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *scriptUrl=order->getParam(1)->toString();//脚本URL

        container::AKArr *outOrderArr=script->getCore()->createArr();
        AKScript *tmpScript=new AKScript(script->getSystem(),script->getCore(),script->getProcess());
        tmpScript->setUrl(scriptUrl);
        tmpScript->loadMidCode();
        AKOrderArray &orderArr=tmpScript->mOrderArray;
        uint32 c=orderArr.size();
        for(uint32 i=0;i<c;++i)
        {
            container::AKArr *tt=script->getCore()->createArr();
            AKOrder *order=orderArr[i];
            uint32 cc=order->params.size();
            for(uint32 ii=0;ii<cc;++ii)
            {
                const int8 *curName=order->params[ii].c_str();
                AKVar *var=tmpScript->getVar(curName);
                if(ii==0&&memcmp(curName,"__sys__",7)==0&&var)
                {
                    AKOrderMap::iterator it=script->mOrderDefineMap.find(curName);
                    if(it==script->mOrderDefineMap.end())//不是内置API
                    {
                        std::string tmpStr="\"";
                        tmpStr+=var->toStdString();
                        tmpStr+="\"";
                        tt->push(tmpStr.c_str());
                    }
                    else//内置API
                    {
                        tt->push(curName);
                    }
                }
                else if(ii>=2)
                {
                    switch(var->getType())
                    {
                        case AKVar::typeNumber:
                            tt->push(var->toString());
                            break;
                        case AKVar::typeString:
                        {
                            std::string tmpStr="\"";
                            std::string val=var->toStdString();
                            AKCommon::str_c_escape(val);
                            tmpStr+=val;
                            tmpStr+="\"";
                            tt->push(tmpStr.c_str());
                        }
                            break;
                        default:
                            tt->push(curName);
                            break;
                    }
                }
                else
                {
                    tt->push(curName);
                }
            }
            outOrderArr->push((void *)tt);
        }
        delete tmpScript;

        out->setValue((void *)outOrderArr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //检查断点是否存在
    bool AKHandleCommon::isBreakPointExist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *scriptUrl=order->getParam(1)->toString();//脚本URL
        uint32 row=order->getParam(2)->toUint();//断点行号

        bool rt=AKGlobal::getSingleton().isBreakPointExist(scriptUrl,row);
        if(rt)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置断点
    bool AKHandleCommon::addBreakPoint(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *scriptUrl=order->getParam(1)->toString();//脚本URL
        uint32 row=order->getParam(2)->toUint();//断点行号

        AKGlobal::getSingleton().addBreakPoint(scriptUrl,row);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除断点
    bool AKHandleCommon::delBreakPoint(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *scriptUrl=order->getParam(1)->toString();//脚本URL
        uint32 row=order->getParam(2)->toUint();//断点行号

        AKGlobal::getSingleton().delBreakPoint(scriptUrl,row);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除所有断点
    bool AKHandleCommon::delAllBreakPoint(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKGlobal::getSingleton().delAllBreakPoint();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取所有变量的名字数组
    bool AKHandleCommon::varNameArr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKArr *arr=script->getCore()->createArr();
        AKVarMap &varMap=script->mVarMap;
        AKVarMap::iterator it=varMap.begin();
        while(it!=varMap.end())
        {
            const std::string &varName=it->first;
            const int8 *varName_c=varName.c_str();
            if(memcmp(varName_c,"__sys__",7)!=0)//过滤临时变量
            {
                arr->push(varName_c);
            }
            ++it;
        }

        out->setValue(arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建调用堆栈中变量的引用
    bool AKHandleCommon::link(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        const int8 *varName=order->getParam(1)->toString();

        AKVar *var=NULL;
        AKScript *curScript=script->getParentScript();
        while(curScript)
        {
            var=curScript->getVar(varName);
            if(var)break;
            curScript=curScript->getParentScript();
        }

        if(var)//变量存在
        {
            out->ref(var,true);
        }
        else//不存在
        {
            onRunError(order,"变量不存在");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取变量值（根据变量名）
    bool AKHandleCommon::var(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        const int8 *varName=order->getParam(1)->toString();
        AKVar *var=script->getVar(varName);
        if(var)//变量存在
        {
            out->setType(var->getType());
            out->setValue(var);
        }
        else//不存在
        {
            onRunError(order,"变量不存在");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取当前核的运行时信息
    bool AKHandleCommon::getCoreRunTimeInfo(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string info=script->getCore()->getRunTimeInfo();

        out->setValue(info);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //记录所有错误信息到文件（不管该错误是否被异常捕捉）
    bool AKHandleCommon::errorLog(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *errorLogFilePath=order->getParam(1)->toString();//错误记录文件路径

        AKGlobal::getSingleton().mErrorLogFilePath=errorLogFilePath;

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取平台标识，win或other
    bool AKHandleCommon::platform(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

#ifdef WIN32
        out->setValue((const int8 *)"win");
#else
        out->setValue((const int8 *)"other");
#endif
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //检查字典或数组的键值是否存在
    bool AKHandleCommon::api(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *apiName=order->getParam(1)->toString();
        const int8 *scriptUrl=order->getParam(2)->toString();

        AKGlobal::getSingleton().mAPIMap[apiName]=scriptUrl;

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //指定的值是否存在于数组中
    bool AKHandleCommon::inArr(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        AKVar *var=order->getParam(2);

        bool isInArr=false;

        if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            //连续数组部分
            uint32 c=arr->mVarArray.size();
            for(int i=0;i<c;++i)
            {
                AKVar *curVar=arr->mVarArray[i];
                if(curVar&&curVar->getType()!=AKVar::typeHandle)
                {
                    if(strcmp(var->toString(),curVar->toString())==0)
                    {
                        isInArr=true;
                        goto inArr_end;
                    }
                }
            }

            //字典部分
            AKVarMap::iterator it=arr->mVarMap.begin();
            while(it!=arr->mVarMap.end())
            {
                const std::string &key=it->first;
                AKVar *curVar=it->second;

                if(curVar->getType()!=AKVar::typeHandle)
                {
                    if(strcmp(var->toString(),curVar->toString())==0)
                    {
                        isInArr=true;
                        goto inArr_end;
                    }
                }

                ++it;
            }
        }
        else
        {
            onRunError(order,"无效容器，必须为数组");
            return false;//暂停执行脚本
        }

        inArr_end:
        if(isInArr)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除字典或数组元素
    bool AKHandleCommon::remove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;

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
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;

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
        }
        else
        {
            onRunError(order,"无效容器，必须为字典或数组");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //清空缓冲、字典或数组
    bool AKHandleCommon::clean(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;
            dict->clear();
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            arr->clear();
        }
        else if(script->getCore()->checkBuf(ref))//缓冲
        {
            container::AKBuf *buf=(container::AKBuf *)ref;
            buf->clear();
        }
        else
        {
            onRunError(order,"无效容器，必须为缓冲、字典或数组");
            return false;//暂停执行脚本
        }

        out->setValue((void *)ref);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //检查字典或数组的键值是否存在
    bool AKHandleCommon::exist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        int8 error[1024];
        AKVar *var=AKCommon::getVarFromContainer(script,order,error);
        if(var)
        {
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //压入字典或数组多个值
    bool AKHandleCommon::push(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;
            //添加字典元素
            for(uint32 i=2;order->getParam(i);i+=2)
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
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            uint32 m=order->varParams.size();
            for(uint32 i=2;i<m;++i)
            {
                arr->push(order->getParam(i));
            }
        }
        else
        {
            onRunError(order,"无效容器，必须为字典或数组");
            return false;//暂停执行脚本
        }

        out->setValue((void *)ref);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //相对路径转绝对路径
    bool AKHandleCommon::full(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        std::string url=AKCommon::scriptUrlFormat(script,order->getParam(1)->toString());

        out->setValue(url);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取命令行参数数量
    bool AKHandleCommon::myargc(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        out->setValue((double)AKCommon::argc);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取命令行参数
    bool AKHandleCommon::myargv(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        uint32 i=order->getParam(1)->toUint();

        if(i<AKCommon::argc)out->setValue((const int8 *)AKCommon::argv[i]);
        else out->setValue((const int8 *)"");
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //输出所有API帮助信息到文件
    bool AKHandleCommon::help(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        std::string filePath="help.txt";
        if(order->getParam(1))
        {
            filePath=order->getParam(1)->toString();
        }

        FILE *fp=fopen(filePath.c_str(),"wb");
        if(fp)
        {
            std::string str=script->help();
            fwrite(str.c_str(),str.size(),1,fp);
            fclose(fp);
        }

        fp=fopen("helpForNodePad.xml","wb");
        if(fp)
        {
            std::string str=script->helpForNotePad();
            fwrite(str.c_str(),str.size(),1,fp);
            fclose(fp);
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //克隆变量或嵌套容器
    bool AKHandleCommon::clone(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        AKVar *var=order->getParam(1);//被克隆的对象

        std::string error;
        bool rt=AKCommon::clone(script->getCore(),out,var,error);
        if(!rt)
        {
            onRunError(order,error.c_str());
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字典、数组或缓冲的尺寸
    bool AKHandleCommon::size(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        uint32 size=0;
        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;
            size=dict->size();
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            size=arr->size();
        }
        else if(script->getCore()->checkBuf(ref))//缓冲
        {
            container::AKBuf *buf=(container::AKBuf *)ref;
            size=buf->size();
        }
        else//空句柄或不支持的容器类型
        {
            onRunError(order,"空句柄或不支持的容器类型");
            return false;//暂停执行脚本
        }

        out->setValue((double)size);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字典或数组的值的引用
    bool AKHandleCommon::getRef(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        int8 error[1024];
        AKVar *var=AKCommon::getVarFromContainer(script,order,error);
        if(!var)
        {
            onRunError(order,error);
            return false;//暂停执行脚本
        }

        out->ref(var,true);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字典或数组的值
    bool AKHandleCommon::get(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        int8 error[1024];
        AKVar *var=AKCommon::getVarFromContainer(script,order,error);
        if(!var)
        {
            onRunError(order,error);
            return false;//暂停执行脚本
        }

        out->setValue(var);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置字典或数组的值
    bool AKHandleCommon::set(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        AKVar *val=order->getParam(3);

        int8 error[1024];
        bool rt=AKCommon::setVarToContainer(script,order,error);
        if(!rt)
        {
            onRunError(order,error);
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //脚本热更新
    bool AKHandleCommon::hotUpdate(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKXmlMgr::getSingleton().clear();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取脚本目录
    bool AKHandleCommon::scriptDir(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        out->setValue((const int8 *)script->getDir());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取脚本URL
    bool AKHandleCommon::scriptUrl(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        out->setValue((const int8 *)script->getUrl());
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取容器引用计数
    bool AKHandleCommon::refCount(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        uint32 refCount=0;
        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;
            refCount=dict->getRefCount();
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            refCount=arr->getRefCount();
        }
        else if(script->getCore()->checkBuf(ref))//缓冲
        {
            container::AKBuf *buf=(container::AKBuf *)ref;
            refCount=buf->getRefCount();
        }
        else//空句柄或不支持的容器类型
        {
            onRunError(order,"空句柄或不支持的容器类型");
            return false;//暂停执行脚本
        }

        out->setValue((double)refCount);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建引用
    bool AKHandleCommon::_ref(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeRef);//设置返回类型

        out->ref(order->getParam(1),true);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置系统自动休眠时间（毫秒）
    bool AKHandleCommon::autoSleep(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        script->getSystem()->mAutoSleepTime=order->getParam(1)->toNumber();

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置是否全速执行当前脚本
    bool AKHandleCommon::fullRun(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        uint32 isFullRun=order->getParam(1)->toUint();
        if(isFullRun)script->mIsFullRun=true;
        else isFullRun=false;

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //跟踪打印信息
    bool AKHandleCommon::trace(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        if(order->getParam(1)->getType()==AKVar::typeHandle)
        {
            int64 handle=(int64)order->getParam(1)->toHandle();
            int8 tmp[1024];
            sprintf(tmp,"[handle %lld]",(int64)handle);
            script->getSystem()->onTrace(tmp);
        }
        else
        {
            script->getSystem()->onTrace(order->getParam(1)->toString());
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //跟踪打印信息
    bool AKHandleCommon::echo(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        printf(order->getParam(1)->toString());

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //退出系统
    bool AKHandleCommon::quitSystem(AKScript *script,AKOrder *order)
    {
        if(!checkParams(0,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        //script->getSystem()->quit();
        exit(0);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除全局变量
    bool AKHandleCommon::gRemove(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=&script->getCore()->mGlobalVarDict;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            dict->remove(index->toUint());
        }
        else if(index->getType()==AKVar::typeString)
        {
            dict->remove(index->toString());
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //全局变量是否存在
    bool AKHandleCommon::gExist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);

        container::AKDict *dict=&script->getCore()->mGlobalVarDict;
        AKVar *var=NULL;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            var=dict->get(index->toUint());
            if(var)
            {
                out->setValue((double)1);
            }
            else
            {
                out->setValue((double)0);
            }
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=dict->get(index->toString());
            if(var)
            {
                out->setValue((double)1);
            }
            else
            {
                out->setValue((double)0);
            }
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取全局变量值
    bool AKHandleCommon::gGet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->clearType();//清除类型（返回值的类型是可变类型）

        container::AKDict *dict=&script->getCore()->mGlobalVarDict;
        AKVar *var=NULL;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            var=dict->get(index->toUint());
            if(var)
            {
                out->setValue(var);
            }
            else
            {
                out->setType(AKVar::typeString);
                out->setValue("");
            }
        }
        else if(index->getType()==AKVar::typeString)
        {
            var=dict->get(index->toString());
            if(var)
            {
                out->setValue(var);
            }
            else
            {
                out->setType(AKVar::typeString);
                out->setValue("");
            }
        }
        else//变量未初始化
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置全局变量值
    bool AKHandleCommon::gSet(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        container::AKDict *dict=&script->getCore()->mGlobalVarDict;
        AKVar *index=order->getParam(1);
        if(index->getType()==AKVar::typeNumber)
        {
            dict->set(index->toUint(),order->getParam(2));
        }
        else if(index->getType()==AKVar::typeString)
        {
            dict->set(index->toString(),order->getParam(2));
        }
        else//非法变量
        {
            onRunError(order,"无效全局索引");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
