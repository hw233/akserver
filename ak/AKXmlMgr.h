//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKXMLMGR_H
#define AKSERVER_AKXMLMGR_H

#include "ak.h"

namespace ak
{
    //xml管理器类
    class AKXmlMgr
    {
    public:
        AKXmlMgr();
        ~AKXmlMgr();
        AKDefineSingleton(AKXmlMgr);

        //清理
        void clear();

        //获取xml对象
        tinyxml2::XMLDocument *get(
                const int8 *url
        );

    private:
        AKStringXmlMap mXmlMap;
    };
}

#endif //AKSERVER_AKXMLMGR_H
