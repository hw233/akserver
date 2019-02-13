//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEDICT_H
#define AKSERVER_AKHANDLEDICT_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //内置字典指令类
    class AKHandleDict:public AKHandler
    {
    public:
        AKHandleDict();
        ~AKHandleDict();
        AKDefineCreate(AKHandleDict);

        virtual void init(AKScript *script);
        virtual void reset();

        container::AKDictIterator *createDictIt();//创建字典迭代器
        bool checkDictIt(container::AKDictIterator *it);//检测字典迭代器有效性

        container::AKDict *createDictFromXml(//根据xml创建字典
                tinyxml2::XMLDocument *xml,
                const int8 *keyName,//作为索引的键名
                const int8 *rowName//行节点名
        );

    public:
        AKDefineScriptFunc(dictDestroy);
        AKDefineScriptFunc(dictCreate);
        AKDefineScriptFunc(dictSize);
        AKDefineScriptFunc(dictClear);
        AKDefineScriptFunc(dictSet);
        AKDefineScriptFunc(dictGet);
        AKDefineScriptFunc(dictExist);
        AKDefineScriptFunc(dictGetRef);
        AKDefineScriptFunc(dictRemove);
        AKDefineScriptFunc(isDict);
        AKDefineScriptFunc(dictIt);
        AKDefineScriptFunc(dictBegin);
        AKDefineScriptFunc(dictEnd);
        AKDefineScriptFunc(dictNext);
        AKDefineScriptFunc(dictPre);
        AKDefineScriptFunc(dictItEmpty);
        AKDefineScriptFunc(dictItKey);
        AKDefineScriptFunc(dictItVal);

        AKDefineScriptFunc(dictCreateFromXml);

    private:
        AKDictIteratorMap mDictIteratorMap;
        AKHandleDict *mParent;
    };
}

#endif //AKSERVER_AKHANDLEDICT_H
