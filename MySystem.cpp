#include "ak.h"
#include "MySystem.h"

MySystem::MySystem(
        uint32 coreCount//核数
)
        :AKSystem(coreCount)
{
}

MySystem::~MySystem()
{
}

//更新
void MySystem::update(double dt)
{
    AKSystem::update(dt);
}

void MySystem::onRunError(const int8 *error)
{
#ifdef WIN32
    printf("%s\r\n",error);
#endif

    //记录错误日志
    FILE *fp=fopen("error.log","ab");
    if(fp)
    {
        fwrite(error,strlen(error),1,fp);
        fwrite("\r\n",2,1,fp);
        fclose(fp);
    }
    exit(-1);
}

void MySystem::onRunCatch(const int8 *error)
{
}

void MySystem::onTrace(const int8 *msg)
{
//#ifdef WIN32
    printf("%s\r\n",msg);
//#endif
}

void MySystem::onRegHandler(AKScript *script)
{
    /*script->regHandler(HandleEngineFunc::create());
    script->regHandler(HandleNodeFunc::create());
    script->regHandler(HandleWebFunc::create());*/
}