#include "ak.h"

//检测内存泄露
/*#ifndef X64
#	ifdef WIN32
#		ifdef _DEBUG
#			define WIN32_LEAN_AND_MEAN
#			include <vld.h>
#		endif
#	endif
#endif*/

#include "MySystem.h"

#ifdef __linux__
#include <sys/prctl.h>
#endif

#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

void init_daemon()
{
    int pid;
    int i;

    if(pid=fork())exit(0);//是父进程，结束父进程
    else if(pid<0)exit(1);//fork失败，退出

    //是第一子进程，后台继续执行
    setsid();//第一子进程成为新的会话组长和进程组长

    //并与控制终端分离
    if(pid=fork())exit(0);//是第一子进程，结束第一子进程
    else if(pid< 0)exit(1);//fork失败，退出

    //是第二子进程，继续
    //第二子进程不再是会话组长

    for(i=0;i< NOFILE;++i)close(i);//关闭打开的文件描述符
    //chdir("/tmp");//改变工作目录到/tmp
    umask(0);//重设文件创建掩模
}
#endif

int main(int32 argc,int8 **argv)
{
    AKCommon::argc=argc;
    AKCommon::argv=argv;

    int8 *configUrl="config.xml";
    if(argc>=2)configUrl=argv[1];

#ifdef __linux__
    if(argc>=3)prctl(PR_SET_NAME,argv[2],NULL,NULL,NULL);//修改进程名
#endif

    uint32 coreCount=1;
    std::string script;

    std::string ext;
    std::string configUrlStr=configUrl;
    if(configUrlStr.size()>=4)
    {
        ext=configUrlStr.substr(configUrlStr.size()-4,4);
        std::transform(ext.begin(),ext.end(),ext.begin(),::tolower);
    }
    if(ext==".aks")
    {
        script=configUrlStr;
        if(argc>=4)
        {
            coreCount=atoi(argv[3]);
            if(coreCount==0)coreCount=1;
        }

#ifndef WIN32
        if(argc>=5)
        {
            int32 daemonSwitch=atoi(argv[4]);//守护进程开关
            if(daemonSwitch==1)
            {
                init_daemon();
            }
        }
#endif
    }
    else
    {
        tinyxml2::XMLDocument *xml=AKXmlMgr::getSingleton().get(configUrl);
        if(xml)
        {
            const tinyxml2::XMLElement *root=xml->RootElement();
            coreCount=atoi(root->FirstChildElement("coreCount")->GetText());//核数
            script=root->FirstChildElement("script")->GetText();//入口脚本路径
        }
        else
        {
            //printf("加载配置文件 %s 错误\r\n",configUrl);
            printf("usage:\n");
            printf("1:akserver config.xml\n");
            printf("2:akserver xx.aks\n");
            printf("3:akserver xx.aks processName [coreCount daemonSwitch]\n");
            exit(-1);
        }
    }

    MySystem sys(coreCount);
    sys.createProcess(script.c_str());
    //sys.quit();
    sys.getIO().run();

    return 0;
}
