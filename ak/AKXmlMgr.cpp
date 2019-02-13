#include "ak.h"
#include "AKXmlMgr.h"

namespace ak
{
    AKXmlMgr::AKXmlMgr()
    {
    }

    AKXmlMgr::~AKXmlMgr()
    {
        clear();
    }

    void AKXmlMgr::clear()
    {
        AKStringXmlMap::iterator it=mXmlMap.begin();
        while(it!=mXmlMap.end())
        {
            delete it->second;
            ++it;
        }
        mXmlMap.clear();
    }

    tinyxml2::XMLDocument *AKXmlMgr::get(
            const int8 *url
    )
    {
        AKStringXmlMap::iterator it=mXmlMap.find(url);
        if(it!=mXmlMap.end())
        {
            return it->second;
        }

        FILE *fp=fopen(url,"rb");
        if(!fp)
        {
            return NULL;
        }
        fseek(fp,0,SEEK_END);
        uint32 len=ftell(fp);
        int8 *tmp=new int8[len+1];
        fseek(fp,0,SEEK_SET);
        fread(tmp,len,1,fp);
        fclose(fp);
        tmp[len]='\0';

        //替换转义符
        std::string code=tmp;
        AKCommon::strReplace(code,"\"&&\"","\"&amp;&amp;\"");//替换"&&"
        AKCommon::strReplace(code,"\"&\"","\"&amp;\"");//替换"&"

        tinyxml2::XMLDocument *xml=new tinyxml2::XMLDocument();
        xml->SetCondenseWhiteSpace(false);//取消空格压缩
        //xml->LoadFile(url);
        xml->Parse(code.c_str());
        delete [] tmp;
        if(xml->Error())
        {
            delete xml;
            return NULL;
        }

        mXmlMap[url]=xml;
        return xml;
    }
}
