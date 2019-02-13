//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_MYSYSTEM_H
#define AKSERVER_MYSYSTEM_H

//自定义系统类
class MySystem:public AKSystem
{
public:
    MySystem(
            uint32 coreCount=2//核数
    );
    ~MySystem();

public:
    //更新
    virtual void update(double dt);

    //运行时出错信息重定向（通过继承并重写此方法，可重定向错误信息）
    virtual void onRunError(const int8 *error);

    //运行时异常信息重定向（通过继承并重写此方法，可重定向错误信息）
    virtual void onRunCatch(const int8 *error);

    //跟踪打印信息
    virtual void onTrace(const int8 *msg);

    //注册自定义脚本指令处理器
    virtual void onRegHandler(AKScript *script);
};


#endif //AKSERVER_MYSYSTEM_H
