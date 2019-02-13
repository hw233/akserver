//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEXML_H
#define AKSERVER_AKHANDLEXML_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    //xml指令类
    class AKHandleXml:public AKHandler
    {
    public:
        AKHandleXml();
        ~AKHandleXml();
        AKDefineCreate(AKHandleXml);

        tinyxml2::XMLDocument *createXml();
        bool checkXml(tinyxml2::XMLDocument *xml);
        void destroyXml(tinyxml2::XMLDocument *xml);

        virtual void init(AKScript *script);
        virtual void reset();

    public:
        AKDefineScriptFunc(xmlCreateFromFile);
        AKDefineScriptFunc(xmlCreate);
        AKDefineScriptFunc(xmlDestroy);
        AKDefineScriptFunc(xmlGet);
        AKDefineScriptFunc(xmlGetSize);

        AKDefineScriptFunc(json_encode);
        AKDefineScriptFunc(json_decode);

    private:
        AKXmlMap mXmlMap;
        AKHandleXml *mParent;
    };
}

#endif //AKSERVER_AKHANDLEXML_H
