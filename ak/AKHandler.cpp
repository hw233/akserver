#include "ak.h"
#include "AKScript.h"
#include "AKHandler.h"

namespace ak
{
    AKHandler::AKHandler()
            :mScript(NULL)
    {
    }

    AKHandler::~AKHandler()
    {
    }

    void AKHandler::init(AKScript *script)
    {
        mScript=script;
    }

    AKScript *AKHandler::getScript()
    {
        return mScript;
    }

    void AKHandler::reset()
    {
    }

    bool AKHandler::checkParams(uint32 needParamCount,AKScript *script,AKOrder *order,bool isDelayOrder)
    {
        /*if(isDelayOrder&&script->mCallType!=AKScript::callType_RunScript)
        {
            int8 tmp[1024];
            sprintf(tmp,"%s()只允许在runScript中调用",order->params[0].c_str());
            script->getSystem()->onRunError(tmp);
            return false;
        }*/
        if(order->varParams.size()<=needParamCount)
        {
            int8 tmp[1024];
            sprintf(tmp,"%s()参数不足",order->params[0].c_str());
            onRunError(order,tmp);
            return false;
        }
        return true;
    }

    void AKHandler::onRunError(AKOrder *order,const int8 *error)
    {
        //输出错误信息
        //int8 tmp[2048];
        //sprintf(tmp,"%s at row %s api %s() %s",mScript->getUrl(),order->rowNum.c_str(),order->params[0].c_str(),error);
        std::string tmp;
        tmp+=mScript->getUrl();
        tmp+=" at row ";
        tmp+=order->rowNum;
        tmp+=" api ";
        tmp+=order->params[0];
        tmp+=" ";
        tmp+=error;


        //输出指令码
        AKStringArray &params=order->params;
        //int8 tmp2[1024];
        //sprintf(tmp2,"    %s=%s(",params[1].c_str(),params[0].c_str());
        std::string tmp2;
        tmp2+="    ";
        tmp2+=params[1];
        tmp2+="=";
        tmp2+=params[0];
        tmp2+="(";
        uint32 c=params.size();
        for(uint32 i=2;i<c;++i)
        {
            if(i>2)
            {
                //strcat(tmp2,",");
                tmp2+=",";
            }
            //strcat(tmp2,params[i].c_str());
            tmp2+=params[i];
        }
        //strcat(tmp2,");");
        tmp2+=");";
        //strcat(tmp,tmp2);
        tmp+=tmp2;

        //错误预处理
        mScript->getSystem()->onPreError(mScript,tmp.c_str());
    }

    AKHandler *AKHandler::getTopParent()
    {
        AKScript *script=getScript();
        AKScript *topParentScript=script->getTopParentScript();
        AKScript *targetScript=NULL;
        if(topParentScript)targetScript=topParentScript;
        else targetScript=script;

        AKHandler *handler=targetScript->getHandler(mName.c_str());
        return handler;
    }
}
