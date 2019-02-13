#include "ak.h"
#include "AKCommon.h"

namespace ak
{
    int AKCommon::argc=0;
    char **AKCommon::argv=NULL;

    AKCommon::AKCommon()
    {
    }

    AKCommon::~AKCommon()
    {
    }

    //生成uuid
    const std::string AKCommon::uuid()
    {
        boost::uuids::uuid uuid=boost::uuids::random_generator()();
        return boost::uuids::to_string(uuid);
    }

    //crc 32位求和校验
    int32 AKCommon::crc32(const int8 *data,uint32 bytes)
    {
        boost::crc_32_type rt;
        rt.process_bytes(data,bytes);
        return rt.checksum();
    }

    //sha1编码
    std::string AKCommon::sha1(const int8 *data,uint32 bytes)
    {
        int8 tmp[2];
        tmp[1]='\0';
        std::string rt;
        boost::uuids::detail::sha1 sha;
        sha.process_bytes(data,bytes);
        uint32 digest[5];
        sha.get_digest(digest);
        for(uint32 i=0;i<5;++i)
        {
            uint32 tt=digest[i];
            for(uint32 j=0;j<4;++j)
            {
                uint8 ch=((int8 *)&tt)[3-j];

                //写入高4位
                uint8 t=ch/16;
                if(t<=9)tmp[0]='0'+t;
                else tmp[0]='a'+t-10;
                rt+=tmp;

                //写入低4位
                t=ch%16;
                if(t<=9)tmp[0]='0'+t;
                else tmp[0]='a'+t-10;
                rt+=tmp;
            }
        }

        return rt;
    }

    //base64编码
    std::string AKCommon::base64(const std::string &input)
    {
        typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;
        std::stringstream result;
        std::copy(Base64EncodeIterator(input.begin()) , Base64EncodeIterator(input.end()),std::ostream_iterator<char>(result));
        size_t equal_count = (3 - input.length() % 3) % 3;
        for (size_t i = 0; i < equal_count; i++)
        {
            result.put('=');
        }
        return result.str();
    }

    //base64解码
    std::string AKCommon::unbase64(const std::string &input)
    {
        typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;
        std::stringstream result;
        try
        {
            copy(Base64DecodeIterator(input.begin()) , Base64DecodeIterator(input.end()),std::ostream_iterator<char>(result));
        }
        catch(...)
        {
            return "";
        }
        return result.str();
    }

    void AKCommon::fileList(container::AKDict &filePathDict,boost::filesystem::path &findDir)
    {
        if (boost::filesystem::exists(findDir))
        {
            boost::filesystem::directory_iterator itBegin(findDir);
            boost::filesystem::directory_iterator itEnd;
            for (;itBegin!=itEnd;itBegin++)
            {
                boost::filesystem::path curPath=itBegin->path().c_str();
                if(boost::filesystem::is_directory(*itBegin))//目录
                {
                    filePathDict.set(curPath.generic_string().c_str(),(double)0);//目录
                    fileList(filePathDict,curPath);
                }
                else//文件
                {
                    filePathDict.set(curPath.generic_string().c_str(),(double)1);//文件
                }
            }
        }
    }

    std::string AKCommon::dirFormat(std::string path)
    {
        //'\\'转'/'
        strReplace(path,"\\","/");

        //消除"../"
        int32 pos=0;
        while((pos=path.find("/.."))>0)
        {
            int32 pos2=path.rfind("/",pos-1);
            if(pos2<0)pos2=0;
            path.replace(pos2,pos+2-pos2+1,"");
        }

        /*
        //消除开头的所有'/'
        while(path.size()>0&&path.at(0)=='/')
        {
            path.replace(0,1,"");
        }
        */

        //消除"./"
        strReplace(path,"./","");

        return path;
    }

    std::string AKCommon::scriptUrlFormat(AKScript *script,const int8 *scriptUrl)
    {
        if(scriptUrl[0]=='@')//添加父级脚本目录路径和父级脚本名
        {
            std::string newUrl=script->getDir();
            newUrl+=script->getParentName();
            newUrl+=".";
            newUrl+=&scriptUrl[1];
            return newUrl;
        }
        else if(scriptUrl[0]=='?')//添加父级脚本目录路径
        {
            std::string newUrl=script->getDir();
            newUrl+=&scriptUrl[1];
            return newUrl;
        }
        return scriptUrl;
    }

    void AKCommon::strReplace(
            std::string &srcStr,
            std::string findStr,
            std::string replaceStr
    )
    {
        if(findStr=="")return;
        uint32 pos=0;
        uint32 a=findStr.size();
        uint32 b=replaceStr.size();
        while((pos=srcStr.find(findStr,pos))!=-1)
        {
            srcStr.replace(pos,a,replaceStr);
            pos+=b;
        }
    }

    void AKCommon::str_c_escape(
            std::string &srcStr
    )
    {
        strReplace(srcStr,"\\","\\\\");
        strReplace(srcStr,"\"","\\\"");
        strReplace(srcStr,"\r","\\r");
        strReplace(srcStr,"\n","\\n");
        strReplace(srcStr,"\t","\\t");
        strReplace(srcStr,"\0","\\0");
    }

    void AKCommon::str_c_unescape(
            std::string &srcStr
    )
    {
        strReplace(srcStr,"\\\\","\\");
        strReplace(srcStr,"\\\"","\"");
        strReplace(srcStr,"\\r","\r");
        strReplace(srcStr,"\\n","\n");
        strReplace(srcStr,"\\t","\t");
        strReplace(srcStr,"\\0","\0");
    }

    void AKCommon::timeInfo(
            uint32 &yy,//年
            uint32 &mm,//月
            uint32 &dd,//日
            uint32 &h,//时
            uint32 &m,//分
            uint32 &s//秒
    )
    {
        boost::gregorian::date today=boost::gregorian::day_clock::local_day();
        yy=today.year();//年
        mm=today.month();//月
        dd=today.day();//日

        std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
        int32 pos=strTime.find("T");
        h=(strTime.at(pos+1)-'0')*10+strTime.at(pos+2)-'0';//时
        m=(strTime.at(pos+3)-'0')*10+strTime.at(pos+4)-'0';//分
        s=(strTime.at(pos+5)-'0')*10+strTime.at(pos+6)-'0';//秒
    }

    bool AKCommon::createDir(const int8 *dir)
    {
        try
        {
            boost::filesystem::create_directories(dir);
        }
        catch (boost::filesystem::filesystem_error e)
        {
            return false;
        }

        return true;
    }

    void AKCommon::sleep(uint32 dt)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(dt));
    }

    std::string AKCommon::MD5(container::AKBuf &buf)
    {
        int8 tmp[16];
        int8 tmp2[33];
        tmp2[32]='\0';

        ak::MD5 md5;
        md5.beginCal();
        uint32 c=buf.getBlockCount();
        for(uint32 i=0;i<c;++i)
        {
            uint32 blockSize=0;
            const int8 *data=buf.getBlockData(i,blockSize);
            md5.addData(data,blockSize);
        }
        md5.endCal();
        md5.getCode(tmp);

        for(uint32 i=0;i<16;++i)
        {
            uint8 ch=tmp[i];
            uint8 l=ch%16;
            uint8 h=ch/16;
            uint32 ii=i*2;
            if(h>9)tmp2[ii]='a'+(h-10);
            else tmp2[ii]='0'+h;
            ++ii;
            if(l>9)tmp2[ii]='a'+(l-10);
            else tmp2[ii]='0'+l;
        }
        return tmp2;
    }

    std::string AKCommon::MD5(const int8 *data,uint32 len)
    {
        int8 tmp[16];
        int8 tmp2[33];
        tmp2[32]='\0';

        ak::MD5 md5;
        md5.beginCal();
        md5.addData(data,len);
        md5.endCal();
        md5.getCode(tmp);

        for(uint32 i=0;i<16;++i)
        {
            uint8 ch=tmp[i];
            uint8 l=ch%16;
            uint8 h=ch/16;
            uint32 ii=i*2;
            if(h>9)tmp2[ii]='a'+(h-10);
            else tmp2[ii]='0'+h;
            ++ii;
            if(l>9)tmp2[ii]='a'+(l-10);
            else tmp2[ii]='0'+l;
        }
        return tmp2;
    }

    bool AKCommon::isUint(const int8 *str)
    {
        uint32 num=atol(str);
        int8 tmp[50];
        sprintf(tmp,"%lu",num);
        if(strcmp(str,tmp)==0)return true;
        return false;
    }

    std::string AKCommon::domain2IP(std::string domain)
    {
        if(domain=="")return "";
        try
        {
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::resolver resolver(io_service);
            boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),domain,"");

            boost::asio::ip::tcp::resolver::iterator endpoint_iterator=resolver.resolve(query);

            boost::asio::ip::tcp::endpoint end=endpoint_iterator->endpoint();
            boost::asio::ip::address addr=end.address();
            std::string ip=addr.to_string();
            return ip;
        }
        catch(...)
        {
            return "";
        }
    }

    //转换为uint索引
    bool AKCommon::string2UintIndex(
            const int8 *index,
            uint32 &uintIndex
    )
    {
        uintIndex=atoi(index);
        int8 tmp[50];
        sprintf(tmp,"%lu",uintIndex);
        if(strcmp(index,tmp)==0)return true;
        return false;
    }

    //求两点间距离
    double AKCommon::vectLength(
            double x1,double y1,//点1
            double x2,double y2//点2
    )
    {
        return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
    }

    //求点到线的交点
    void AKCommon::pointCrossLine(
            double &xc,double &yc,//交点
            double x1,double y1,//直线上点1
            double x2,double y2,//直线上点2
            double x3,double y3//垂线上的点
    )
    {
        if(x2!=x1&&y2!=y1)
        {
            double a=1/(x2-x1);
            double b=-1/(y2-y1);
            double c=y1/(y2-y1)-x1/(x2-x1);
            xc=(b*b*x3-a*b*y3-a*c)/(a*a+b*b);
            yc=(-a*b*x3+a*a*y3-b*c)/(a*a+b*b);
        }
        else if(x2!=x1&&y2==y1)
        {
            xc=x3;
            yc=y1;
        }
        else if(x2==x1&&y2!=y1)
        {
            xc=x1;
            yc=y3;
        }

    }

    double AKCommon::angle2radian(double angle)
    {
        return(3.1415*angle/180);
    }

    double AKCommon::radian2angle(double radian)
    {
        return(180*radian/3.1415);
    }

    double AKCommon::vectAngle(double x0,double y0,double x1,double y1)
    {
        double angle=0;
        double len=sqrt(pow(x1-x0,2)+pow(y1-y0,2));
        if(len>0)
        {
            double vx=x1-x0;
            double vy=y1-y0;
            double dx=abs(vx);
            double dy=abs(vy);
            vx/=len;
            vy/=len;

            if(vx==0)
            {
                if(vy<0)angle=90;
                else angle=270;
            }
            else if(vy==0)
            {
                if(vx<0)angle=180;
                else angle=0;
            }
            else
            {
                angle=atan(dy/dx)*180/3.1415;
                if(vx>0&&vy<0)
                {
                    //angle=angle;
                }
                else if(vx<0&&vy<0)
                {
                    angle=180-angle;
                }
                else if(vx<0&&vy>0)
                {
                    angle=180+angle;
                }
                else
                {
                    angle=360-angle;
                }
            }
        }

        //以上是使用x轴向右,y轴向下的坐标系统，需要转成x轴向右,y轴向上的坐标系统
        angle=-angle;
        if(angle<0)
        {
            angle=360+angle;
        }

        return angle;
    }

    //计算直线移动的位置
    void AKCommon::moveTo(
            double &x,//开始位置（计算完成后保存结果的变量）
            double &y,//开始位置（计算完成后保存结果的变量）
            double xDest,//目标位置
            double yDest,//目标位置
            double speed,//移动速度
            double dt//经过的时间（毫秒）
    )
    {
        double dx=xDest-x;
        double dy=yDest-y;
        double len=sqrt(dx*dx+dy*dy);
        double step=speed*dt/1000;
        if(step>=len)
        {
            x=xDest;
            y=yDest;
        }
        else
        {
            double percent=step/len;
            x+=dx*percent;
            y+=dy*percent;
        }
    }

    //缓冲转十六进制字符串
    void AKCommon::buf2hex(std::string &hexStr,container::AKBuf *buf)
    {
        hexStr="";

        uint32 oldPos=buf->getPos();

        uint8 t;
        int8 tmp[2];
        tmp[1]='\0';
        buf->setPos(0);
        uint32 bytes=buf->size();
        uint8 ch;
        for(uint32 i=0;i<bytes;++i)
        {
            buf->readUint8(ch);

            //写入高4位
            t=ch/16;
            if(t<=9)tmp[0]='0'+t;
            else tmp[0]='a'+t-10;
            hexStr+=tmp;

            //写入低4位
            t=ch%16;
            if(t<=9)tmp[0]='0'+t;
            else tmp[0]='a'+t-10;
            hexStr+=tmp;
        }

        buf->setPos(oldPos);
    }

    bool AKCommon::hex2buf(container::AKBuf *buf,std::string &hexStr)
    {
        buf->clear();
        std::transform(hexStr.begin(),hexStr.end(),hexStr.begin(),::tolower);
        uint32 hexStrSize=hexStr.size();
        const int8 *str=hexStr.c_str();
        uint8 t,h,l;
        for(uint32 i=0;i<hexStrSize;i+=2)
        {
            //高4位
            t=str[i];
            if(t>='0'&&t<='9')t-='0';
            else if(t>='a'&&t<='f')t=t-'a'+10;
            else
            {
                return false;//无效hexStr
            }
            h=t;

            //低4位
            t=str[i+1];
            if(t>='0'&&t<='9')t-='0';
            else if(t>='a'&&t<='f')t=t-'a'+10;
            else
            {
                return false;//无效hexStr
            }
            l=t;

            //合成字节
            t=(h<<4)|l;

            buf->writeUint8(t);
        }
        buf->setPos(0);
        return true;
    }

    std::string AKCommon::now()
    {
        boost::gregorian::date today=boost::gregorian::day_clock::local_day();
        int32 yy=today.year();//年
        int32 mm=today.month();//月
        int32 dd=today.day();//日

        std::string strTime=boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
        int32 pos=strTime.find(" ");
        const int8 *tt=&strTime.c_str()[pos+1];

        int8 tmp[100];
        sprintf(tmp,"%ld-%02ld-%02ld %s",yy,mm,dd,tt);

        return tmp;
    }

    uint32 getCount(acl::json_node *node)
    {
        if(node->children_count()==0)
        {
            node->clear();
            return 0;
        }
        uint32 c=getCount(node->first_child())+1;
        node->clear();
        return c;
    }

    void AKCommon::json_encode(
            AKCore *core,//核对象
            AKVar *var,//变量
            std::string &outStr//接受输出的字符串
    )
    {
        if(var->getType()==0)//未初始化
        {
            throw "json_encode error";
        }
        else if(var->getType()==AKVar::typeNumber)//数值
        {
            outStr+=var->toString();
        }
        else if(var->getType()==AKVar::typeString)//字符串
        {
            std::string str=var->toString();
            str_c_escape(str);
            outStr+="\"";
            outStr+=str;
            outStr+="\"";
        }
        else if(var->getType()==AKVar::typeHandle)//句柄
        {
            void *handle=var->toHandle();
            if(core->checkArr((AKRef *)handle))//数组
            {
                outStr+="[";
                container::AKArr *arr=(container::AKArr *)handle;
                uint32 c=arr->size();
                for(uint32 i=0;i<c;++i)
                {
                    if(i>0)outStr+=",";
                    AKVar *curVar=arr->get(i);
                    json_encode(core,curVar,outStr);
                }
                outStr+="]";
            }
            else if(core->checkDict((AKRef *)handle))//字典
            {
                outStr+="{";
                int8 tmp[50];
                bool first=true;
                container::AKDict *dict=(container::AKDict *)handle;
                //整数下标字典
                AKUint32VarMap::iterator it=dict->mUint32VarMap.begin();
                while(it!=dict->mUint32VarMap.end())
                {
                    if(first)first=false;
                    else outStr+=",";

                    uint32 key=it->first;
                    AKVar *curVar=it->second;

                    sprintf(tmp,"%lu",key);
                    std::string keyStr=tmp;
                    str_c_escape(keyStr);

                    outStr+="\"";
                    outStr+=keyStr;
                    outStr+="\":";

                    json_encode(core,curVar,outStr);

                    ++it;
                }

                //写入字符串下标字典
                AKVarMap::iterator it2=dict->mVarMap.begin();
                while(it2!=dict->mVarMap.end())
                {
                    if(first)first=false;
                    else outStr+=",";

                    const std::string &key=it2->first;
                    AKVar *curVar=it2->second;

                    std::string keyStr=key;
                    str_c_escape(keyStr);

                    outStr+="\"";
                    outStr+=keyStr;
                    outStr+="\":";

                    json_encode(core,curVar,outStr);

                    ++it2;
                }
                outStr+="}";
            }
            else//其它句柄
            {
                throw "json_encode error";
            }
        }
        else if(var->getType()==AKVar::typeRef)//引用
        {
            json_encode(core,var,outStr);
        }
    }

    //反序列化
    bool AKCommon::unserialize(
            AKCore *core,//核对象
            AKVar *var,//接收结果的变量
            container::AKBuf *buf//序列化数据的缓冲
    )
    {
        if(buf->size()==0)return false;

        //读取类型标识
        uint8 varType=0;
        if(!buf->readUint8(varType))return false;
        switch(varType)
        {
            case AKVar::typeNumber://数值
            {
                double num=0;
                if(!buf->readDouble(num))return false;
                var->setType(AKVar::typeNumber);
                var->setValue((double)num);
                break;
            }
            case AKVar::typeString://字符串
            {
                uint32 bytes=0;
                if(!buf->readUint32(bytes))return false;
                container::AKUseStaticMem mem(core->mStaticMem);//临时缓存
                int8 *tmp=mem.alloc(bytes);
                if(!buf->read(tmp,bytes))
                {
                    return false;
                }
                var->setType(AKVar::typeString);
                var->setValue((const int8 *)tmp);
                break;
            }
            case (AKVar::typeHandle+10)://缓冲
            {
                uint32 bytes=0;
                if(!buf->readUint32(bytes))return false;

                if(bytes>(buf->size()-buf->getPos()))return false;//没有足够的数据读出

                container::AKBuf *newBuf=core->createBuf();
                newBuf->append(*buf,buf->getPos(),bytes);
                buf->setPos(buf->getPos()+bytes);
                newBuf->setPos(0);
                var->setType(AKVar::typeHandle);
                var->setValue((void *)newBuf);
                break;
            }
            case (AKVar::typeHandle+11)://数组
            {
                container::AKArr *newArr=core->createArr();
                uint32 i=0,c=0;

                //连续数组部分
                if(!buf->readUint32(c))return false;//尺寸
                for(i=0;i<c;++i)
                {
                    AKVar *newVar=new AKVar(core);
                    newArr->mVarArray.push_back(newVar);
                    bool rt=unserialize(core,newVar,buf);
                    if(!rt)return rt;
                }

                //字典部分
                if(!buf->readUint32(c))return false;//尺寸
                for(i=0;i<c;++i)
                {
                    //key
                    uint32 bytes=0;
                    if(!buf->readUint32(bytes))return false;
                    container::AKUseStaticMem mem(core->mStaticMem);//临时缓存
                    int8 *tmp=mem.alloc(bytes);
                    if(!buf->read(tmp,bytes))
                    {
                        return false;
                    }
                    std::string key=tmp;

                    //元素
                    AKVar *newVar=new AKVar(core);
                    newArr->mVarMap[key]=newVar;
                    bool rt=unserialize(core,newVar,buf);
                    if(!rt)return rt;
                }

                var->setType(AKVar::typeHandle);
                var->setValue((void *)newArr);
                break;
            }
            case (AKVar::typeHandle+12)://字典
            {
                container::AKDict *newDict=core->createDict();
                uint32 i=0,c=0;

                //数值下标部分
                if(!buf->readUint32(c))return false;//尺寸
                for(i=0;i<c;++i)
                {
                    AKVar *newVar=new AKVar(core);

                    uint32 key=0;
                    if(!buf->readUint32(key))return false;
                    newDict->mUint32VarMap[key]=newVar;

                    bool rt=unserialize(core,newVar,buf);
                    if(!rt)return rt;
                }

                //字符串下标部分
                if(!buf->readUint32(c))return false;//尺寸
                for(i=0;i<c;++i)
                {
                    //key
                    uint32 bytes=0;
                    if(!buf->readUint32(bytes))return false;
                    container::AKUseStaticMem mem(core->mStaticMem);//临时缓存
                    int8 *tmp=mem.alloc(bytes);
                    if(!buf->read(tmp,bytes))
                    {
                        return false;
                    }
                    std::string key=tmp;

                    //元素
                    AKVar *newVar=new AKVar(core);
                    newDict->mVarMap[key]=newVar;
                    bool rt=unserialize(core,newVar,buf);
                    if(!rt)return rt;
                }

                var->setType(AKVar::typeHandle);
                var->setValue((void *)newDict);
                break;
            }
            case (AKVar::typeHandle+13)://进程句柄
            {
                uint32 bytes=0;
                if(!buf->readUint32(bytes))return false;
                container::AKUseStaticMem mem(core->mStaticMem);//临时缓存
                void *handle=NULL;
                if(!buf->read((int8 *)&handle,bytes))
                {
                    return false;
                }
                var->setType(AKVar::typeHandle);
                var->setValue((void *)handle);
                break;
            }
            default://未知类型
            {
                return false;
            }
        }
        return true;
    }

    //序列化
    bool AKCommon::serialize(
            AKCore *core,//核对象
            container::AKBuf *buf,//接收序列化数据的缓冲
            AKVar *var,//需要序列化的变量
            std::string &error//错误信息
    )
    {
        if(var->getType()==0)//未初始化
        {
            error="包含未初始化的变量";
            return false;
        }
        else if(var->getType()==AKVar::typeNumber)//数值
        {
            buf->writeUint8(AKVar::typeNumber);//写入类型标识
            buf->writeDouble(var->toNumber());//写入数值
        }
        else if(var->getType()==AKVar::typeString)//字符串
        {
            uint32 bytes=var->toStringSize()+1;
            buf->writeUint8(AKVar::typeString);//写入类型标识
            buf->writeUint32(bytes);//写入长度
            buf->write(var->toString(),bytes);//写入字符串
        }
        else if(var->getType()==AKVar::typeHandle)//句柄
        {
            void *handle=var->toHandle();
            if(core->checkBuf((AKRef *)handle))//缓冲
            {
                container::AKBuf *curBuf=(container::AKBuf *)handle;
                uint32 bytes=curBuf->size();
                buf->writeUint8(AKVar::typeHandle+10);//写入类型标识
                buf->writeUint32(bytes);//写入长度
                buf->append(*curBuf);//写入数据
            }
            else if(core->checkArr((AKRef *)handle))//数组
            {
                buf->writeUint8(AKVar::typeHandle+11);//写入类型标识
                container::AKArr *arr=(container::AKArr *)handle;
                uint32 i;

                //写入连续数组部分
                uint32 c=arr->mVarArray.size();
                buf->writeUint32(c);//写入连续数组尺寸
                for(i=0;i<c;++i)
                {
                    AKVar *curVar=arr->mVarArray[i];
                    if(!curVar)
                    {
                        error="包含的数组存在不连续";
                        return false;
                    }

                    //写入元素
                    bool rt=serialize(core,buf,curVar,error);
                    if(!rt)return rt;
                }

                //写入字典部分
                c=arr->mVarMap.size();
                buf->writeUint32(c);//写入字典尺寸
                AKVarMap::iterator it=arr->mVarMap.begin();
                while(it!=arr->mVarMap.end())
                {
                    const std::string &key=it->first;
                    AKVar *curVar=it->second;
                    uint32 bytes=key.size()+1;

                    //写入key
                    buf->writeUint32(bytes);//写入长度
                    buf->write(key.c_str(),bytes);//写入字符串

                    //写入元素
                    bool rt=serialize(core,buf,curVar,error);
                    if(!rt)return rt;
                    ++it;
                }
            }
            else if(core->checkDict((AKRef *)handle))//字典
            {
                buf->writeUint8(AKVar::typeHandle+12);//写入类型标识
                container::AKDict *dict=(container::AKDict *)handle;

                //写入整数下标字典
                uint32 c=dict->mUint32VarMap.size();
                buf->writeUint32(c);//写入字典尺寸
                AKUint32VarMap::iterator it=dict->mUint32VarMap.begin();
                while(it!=dict->mUint32VarMap.end())
                {
                    uint32 key=it->first;
                    AKVar *curVar=it->second;

                    //写入key
                    buf->writeUint32(key);

                    //写入元素
                    bool rt=serialize(core,buf,curVar,error);
                    if(!rt)return rt;
                    ++it;
                }

                //写入字符串下标字典
                c=dict->mVarMap.size();
                buf->writeUint32(c);//写入字典尺寸
                AKVarMap::iterator it2=dict->mVarMap.begin();
                while(it2!=dict->mVarMap.end())
                {
                    const std::string &key=it2->first;
                    AKVar *curVar=it2->second;
                    uint32 bytes=key.size()+1;

                    //写入key
                    buf->writeUint32(bytes);//写入长度
                    buf->write(key.c_str(),bytes);//写入字符串

                    //写入元素
                    bool rt=serialize(core,buf,curVar,error);
                    if(!rt)return rt;
                    ++it2;
                }
            }
            else if(core->checkProcess((AKProcess *)handle))//进程
            {
                uint32 bytes=sizeof(void *);
                buf->writeUint8(AKVar::typeHandle+13);//写入类型标识
                buf->writeUint32(bytes);//写入长度
                buf->write((const int8 *)&handle,bytes);//写入进程句柄
            }
            else//其它句柄
            {
                error="包含不支持的句柄";
                return false;
            }
        }
        else if(var->getType()==AKVar::typeRef)//引用
        {
            return serialize(core,buf,var->getInst(),error);
        }
        return true;
    }

    //克隆
    bool AKCommon::clone(
            AKCore *core,//核对象
            AKVar *newVar,//接收对象
            AKVar *var,//被克隆的对象
            std::string &error//错误信息
    )
    {
        if(var->getType()==0)//未初始化
        {
            error="包含未初始化的变量";
            return false;
        }
        else if(var->getType()==AKVar::typeNumber)//数值
        {
            newVar->setType(AKVar::typeNumber);
            newVar->setValue((double)var->toNumber());
        }
        else if(var->getType()==AKVar::typeString)//字符串
        {
            newVar->setType(AKVar::typeString);
            newVar->setValue((const int8 *)var->toString());
        }
        else if(var->getType()==AKVar::typeHandle)//句柄
        {
            AKRef *ref=(AKRef *)var->toHandle();
            if(core->checkBuf(ref))//缓冲
            {
                container::AKBuf *curBuf=(container::AKBuf *)ref;
                container::AKBuf *newBuf=core->createBuf();
                newBuf->append(*curBuf);
                newVar->setType(AKVar::typeHandle);
                newVar->setValue((void *)newBuf);
            }
            else if(core->checkArr((AKRef *)var->toHandle()))//数组
            {
                container::AKArr *arr=(container::AKArr *)ref;
                container::AKArr *newArr=core->createArr();

                uint32 i;

                //克隆连续数组部分
                uint32 c=arr->mVarArray.size();
                for(i=0;i<c;++i)
                {
                    AKVar *curVar=arr->mVarArray[i];
                    if(!curVar)
                    {
                        error="包含的数组存在不连续";
                        return false;
                    }
                    AKVar *newVar=new AKVar(core);
                    newArr->mVarArray.push_back(newVar);
                    bool rt=clone(core,newVar,curVar,error);
                    if(!rt)return rt;
                }

                //克隆字典部分
                c=arr->mVarMap.size();
                AKVarMap::iterator it=arr->mVarMap.begin();
                while(it!=arr->mVarMap.end())
                {
                    const std::string &key=it->first;
                    AKVar *curVar=it->second;
                    AKVar *newVar=new AKVar(core);
                    newArr->mVarMap[key]=newVar;
                    bool rt=clone(core,newVar,curVar,error);
                    if(!rt)return rt;
                    ++it;
                }

                newVar->setType(AKVar::typeHandle);
                newVar->setValue((void *)newArr);
            }
            else if(core->checkDict((AKRef *)var->toHandle()))//字典
            {
                container::AKDict *dict=(container::AKDict *)ref;
                container::AKDict *newDict=core->createDict();

                //克隆整数下标字典
                uint32 c=dict->mUint32VarMap.size();
                AKUint32VarMap::iterator it=dict->mUint32VarMap.begin();
                while(it!=dict->mUint32VarMap.end())
                {
                    uint32 key=it->first;
                    AKVar *curVar=it->second;
                    AKVar *newVar=new AKVar(core);
                    newDict->mUint32VarMap[key]=newVar;
                    bool rt=clone(core,newVar,curVar,error);
                    if(!rt)return rt;
                    ++it;
                }

                //写入字符串下标字典
                c=dict->mVarMap.size();
                AKVarMap::iterator it2=dict->mVarMap.begin();
                while(it2!=dict->mVarMap.end())
                {
                    const std::string &key=it2->first;
                    AKVar *curVar=it2->second;
                    AKVar *newVar=new AKVar(core);
                    newDict->mVarMap[key]=newVar;
                    bool rt=clone(core,newVar,curVar,error);
                    if(!rt)return rt;
                    ++it2;
                }

                newVar->setType(AKVar::typeHandle);
                newVar->setValue((void *)newDict);
            }
            else//其它句柄
            {
                newVar->setType(AKVar::typeHandle);
                newVar->setValue((void *)var->toHandle());
            }
        }
        else if(var->getType()==AKVar::typeRef)//引用
        {
            return clone(core,newVar,var->getInst(),error);
        }
        return true;
    }

    bool AKCommon::setVarToContainer(
            AKScript *script,
            AKOrder *order,
            int8 *error
    )
    {
        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        AKVar *index=order->getParam(2);
        AKVar *val=order->getParam(3);

        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;
            if(index->getType()==AKVar::typeNumber)
            {
                dict->set(index->toUint(),val);
            }
            else if(index->getType()==AKVar::typeString)
            {
                dict->set(index->toString(),val);
            }
            else//非法变量
            {
                strcpy(error,"无效字典索引");
                return false;//暂停执行脚本
            }
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            if(index->getType()==AKVar::typeNumber)
            {
                arr->set(index->toUint(),val);
            }
            else if(index->getType()==AKVar::typeString)
            {
                arr->set(index->toString(),val);
            }
            else//非法变量
            {
                strcpy(error,"无效索引");
                return false;//暂停执行脚本
            }
        }
        else//空句柄或不支持的容器类型
        {
            strcpy(error,"空句柄或不支持的容器类型");
            return false;//暂停执行脚本
        }

        return true;
    }

    AKVar *AKCommon::getVarFromContainer(
            AKScript *script,
            AKOrder *order,
            int8 *error,
            bool isAutoCreateVar
    )
    {
        AKRef *ref=(AKRef *)order->getParam(1)->toHandle();
        AKVar *var=NULL;
        if(script->getCore()->checkDict(ref))//字典
        {
            container::AKDict *dict=(container::AKDict *)ref;
            AKVar *index=NULL;
            for(uint32 i=2;order->getParam(i);++i)
            {
                if(!dict)
                {
                    dict=(container::AKDict *)var->toHandle();
                }
                if(!script->getCore()->checkDict(dict))
                {
                    strcpy(error,"无效字典");
                    return NULL;//暂停执行脚本
                }
                index=order->getParam(i);
                if(index->getType()==AKVar::typeNumber)
                {
                    var=dict->get(index->toUint());
                }
                else if(index->getType()==AKVar::typeString)
                {
                    var=dict->get(index->toString());
                }
                else//变量未初始化
                {
                    strcpy(error,"无效字典索引");
                    return NULL;//暂停执行脚本
                }
                if(isAutoCreateVar&&var==NULL)//自动创建变量
                {
                    if(index->getType()==AKVar::typeNumber)
                    {
                        var=new AKVar(script->getCore());
                        dict->mUint32VarMap[index->toUint()]=var;
                    }
                    else if(index->getType()==AKVar::typeString)
                    {
                        uint32 uintIndex;
                        bool rt=AKCommon::string2UintIndex(index->toString(),uintIndex);
                        if(rt)//数值下标
                        {
                            var=new AKVar(script->getCore());
                            dict->mUint32VarMap[uintIndex]=var;
                        }
                        else//字符串下标
                        {
                            var=new AKVar(script->getCore());
                            dict->mVarMap[index->toString()]=var;
                        }
                    }
                }
                else if(!var)
                {
                    strcpy(error,"字典索引查找失败");
                    return NULL;//暂停执行脚本
                }
                dict=NULL;//换下一个字典
            }
        }
        else if(script->getCore()->checkArr(ref))//数组
        {
            container::AKArr *arr=(container::AKArr *)ref;
            AKVar *index=order->getParam(2);
            if(index->getType()==AKVar::typeNumber)
            {
                var=arr->get(index->toUint());
            }
            else if(index->getType()==AKVar::typeString)
            {
                var=arr->get(index->toString());
            }
            else//变量未初始化
            {
                strcpy(error,"无效索引");
                return NULL;//暂停执行脚本
            }

            if(isAutoCreateVar&&var==NULL)//自动创建变量
            {
                if(index->getType()==AKVar::typeNumber)
                {
                    arr->set(index->toUint(),var=new AKVar(script->getCore()));
                }
                else if(index->getType()==AKVar::typeString)
                {
                    arr->set(index->toString(),var=new AKVar(script->getCore()));
                }
            }
            else if(!var)
            {
                strcpy(error,"数组索引查找失败");
                return NULL;//暂停执行脚本
            }
        }
        else//空句柄或不支持的容器类型
        {
            strcpy(error,"空句柄或不支持的容器类型");
            return NULL;//暂停执行脚本
        }

        return var;
    }

    void AKCommon::log(
            const int8 *filePath,
            const int8 *logStr
    )
    {
        FILE *fp=fopen(filePath,"ab");
        if(fp)
        {
            fwrite(logStr,strlen(logStr),1,fp);
            fclose(fp);
        }
    }

    void AKCommon::log(
            const int8 *filePath,
            std::string &logStr
    )
    {
        FILE *fp=fopen(filePath,"ab");
        if(fp)
        {
            fwrite(logStr.c_str(),logStr.size(),1,fp);
            fclose(fp);
        }
    }

    //返回时间整数（秒）
    double AKCommon::time()
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::gregorian::Jan,1));
        boost::posix_time::time_duration time_from_epoch=boost::posix_time::microsec_clock::universal_time()-epoch;
        return time_from_epoch.total_seconds();
    }

    //返回时间整数（毫秒）
    double AKCommon::time_milli()
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::gregorian::Jan,1));
        boost::posix_time::time_duration time_from_epoch=boost::posix_time::microsec_clock::universal_time()-epoch;
        return time_from_epoch.total_milliseconds();
    }

    //返回时间整数（微秒）
    double AKCommon::time_micro()
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::gregorian::Jan,1));
        boost::posix_time::time_duration time_from_epoch=boost::posix_time::microsec_clock::universal_time()-epoch;
        return time_from_epoch.total_microseconds();
    }

    //返回时间整数（纳秒）
    double AKCommon::time_nano()
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::gregorian::Jan,1));
        boost::posix_time::time_duration time_from_epoch=boost::posix_time::microsec_clock::universal_time()-epoch;
        return time_from_epoch.total_nanoseconds();
    }

    unsigned char ToHex(unsigned char x)
    {
        return  x > 9 ? x + 55 : x + 48;
    }

    unsigned char FromHex(unsigned char x)
    {
        unsigned char y;
        if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
        else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
        else if (x >= '0' && x <= '9') y = x - '0';
        else assert(0);
        return y;
    }

    std::string AKCommon::urlencode(const std::string &str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (isalnum((unsigned char)str[i])||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~'))
                strTemp += str[i];
            else if (str[i] == ' ')
                strTemp += "+";
            else
            {
                strTemp += '%';
                strTemp += ToHex((unsigned char)str[i] >> 4);
                strTemp += ToHex((unsigned char)str[i] % 16);
            }
        }
        return strTemp;
    }

    std::string AKCommon::urldecode(const std::string &str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (str[i] == '+') strTemp += ' ';
            else if (str[i] == '%')
            {
                assert(i + 2 < length);
                unsigned char high = FromHex((unsigned char)str[++i]);
                unsigned char low = FromHex((unsigned char)str[++i]);
                strTemp += high*16 + low;
            }
            else strTemp += str[i];
        }
        return strTemp;
    }

    void AKCommon::split(
            AKStringList &strList,//分割后保存的列表
            const int8 *str,//源字符串
            const int8 *spliter//分割字符串
    )
    {
        //分割成数组
        int32 spliterLen=strlen(spliter);
        std::string srcStr=str;
        int32 pos=0;
        while(true)
        {
            int32 pos2=srcStr.find(spliter,pos);
            if(pos2>=0)
            {
                strList.push_back(srcStr.substr(pos,pos2-pos).c_str());
                pos=pos2+spliterLen;
                if(pos>=srcStr.size())break;
            }
            else
            {
                strList.push_back(srcStr.substr(pos).c_str());
                break;
            }
        }
    }

    std::string AKCommon::timetostr(time_t time)
    {
        struct tm *l=localtime(&time);
        char tmp[128];
        sprintf(tmp,"%04d-%02d-%02d %02d:%02d:%02d",l->tm_year+1900,l->tm_mon+1,l->tm_mday,l->tm_hour,l->tm_min,l->tm_sec);
        return tmp;
    }

}
