#include "ak.h"
#include "AKStruct.h"

namespace ak
{
    namespace container
    {
        AKStruct::AKStruct()
        {
            //运算符列表
            m_OperatorList.push_back("<");
            m_OperatorList.push_back(">");
            m_OperatorList.push_back("{");
            m_OperatorList.push_back("}");
            m_OperatorList.push_back(":");
            m_OperatorList.push_back("=");
            m_OperatorList.push_back(";");
            m_OperatorList.push_back("struct");
            m_OperatorList.push_back("int8");
            m_OperatorList.push_back("uint8");
            m_OperatorList.push_back("int16");
            m_OperatorList.push_back("uint16");
            m_OperatorList.push_back("int32");
            m_OperatorList.push_back("uint32");
            m_OperatorList.push_back("int64");
            m_OperatorList.push_back("uint64");
            m_OperatorList.push_back("float");
            m_OperatorList.push_back("double");
            m_OperatorList.push_back("string");
            m_OperatorList.push_back("array");
            m_OperatorList.push_back("dict");
            //类型列表
            typeMap["int8"]=1;
            typeMap["uint8"]=2;
            typeMap["int16"]=3;
            typeMap["uint16"]=4;
            typeMap["int32"]=5;
            typeMap["uint32"]=6;
            typeMap["int64"]=7;
            typeMap["uint64"]=8;
            typeMap["float"]=20;
            typeMap["double"]=21;
            typeMap["string"]=30;
            typeMap["array"]=40;
            typeMap["dict"]=41;
        }

        //是否为指定运算符
        bool AKStruct::isOperator(Word &word,const int8 *name)
        {
            if(word.type==Word::OPERATOR&&word.val==name)return true;
            return false;
        }

        //处理值类型成员，类型:1=int8;2=uint8;3=int16;4=uint16;5=int32;6=uint32;7=int64;8=uint64;20=float;21=double;30=string;
        bool AKStruct::handleValMember(const int8 *structStr,uint32 &pos,Struct &structInfo,uint32 memberType)
        {
            Word word;

            //成员名称
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::SIGN_NAME)return false;//语法错误
            std::string memberName=word.val;
            if(structInfo.memberMap.find(memberName)!=structInfo.memberMap.end())return false;//成员名称重复
            Member &member=structInfo.memberMap[memberName];
            member.name=memberName;
            member.type=memberType;

            //键索引开始
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,":"))return false;//语法错误

            //键索引值
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::NUMBER)return false;//语法错误
            member.keyIndex=atoi(word.val.c_str());
            if(structInfo.memberIndexMap.find(member.keyIndex)!=structInfo.memberIndexMap.end())return false;//键索引重复
            structInfo.memberIndexMap[member.keyIndex]=&member;

            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(isOperator(word,";"))//成员定义结束
            {
            }
            else if(isOperator(word,"="))//成员默认值开始
            {
                //成员默认值
                if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
                if(word.type!=Word::NUMBER&&word.type!=Word::STRING)return false;//语法错误
                member.val=word.val;

                //成员定义结束
                if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
                if(!isOperator(word,";"))return false;//语法错误
            }
            else//语法错误
            {
                return false;
            }

            return true;//成功
        }

        //处理结构体对象成员
        bool AKStruct::handleObject(const int8 *structStr,uint32 &pos,Struct &structInfo,std::string structName)
        {
            Word word;

            //成员名称
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::SIGN_NAME)return false;//语法错误
            std::string memberName=word.val;
            if(structInfo.memberMap.find(memberName)!=structInfo.memberMap.end())return false;//成员名称重复
            Member &member=structInfo.memberMap[memberName];
            member.name=memberName;
            member.type=50;
            member.val=structName;

            //键索引开始
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,":"))return false;//语法错误

            //键索引值
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::NUMBER)return false;//语法错误
            member.keyIndex=atoi(word.val.c_str());
            if(structInfo.memberIndexMap.find(member.keyIndex)!=structInfo.memberIndexMap.end())return false;//键索引重复
            structInfo.memberIndexMap[member.keyIndex]=&member;

            //成员定义结束
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,";"))return false;//语法错误

            return true;
        }

        //处理数组成员
        bool AKStruct::handleArray(const int8 *structStr,uint32 &pos,Struct &structInfo)
        {
            Word word;
            std::string typeName;//数组类型

            //数组类型开始
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,"<"))return false;//语法错误

            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type==Word::OPERATOR)//基本类型数组
            {
                uint32 type=getBaseType(word.val);
                if(type>=1&&type<=30)//数值或字符串
                {
                    typeName=word.val;
                }
                else//基础类型错误
                {
                    return false;
                }
            }
            else if(word.type==Word::SIGN_NAME)//结构体数据
            {
                StructMap::iterator it=structMap.find(word.val);
                if(it==structMap.end())return false;//结构体不存在
                typeName=word.val;
            }

            //数组类型结束
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,">"))return false;//语法错误

            //成员名称
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::SIGN_NAME)return false;//语法错误
            std::string memberName=word.val;
            uint32 type=getBaseType(word.val);
            if(structInfo.memberMap.find(memberName)!=structInfo.memberMap.end())return false;//成员名称重复
            Member &member=structInfo.memberMap[memberName];
            member.name=memberName;
            member.type=40;
            member.val=typeName;

            //键索引开始
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,":"))return false;//语法错误

            //键索引值
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::NUMBER)return false;//语法错误
            member.keyIndex=atoi(word.val.c_str());
            if(structInfo.memberIndexMap.find(member.keyIndex)!=structInfo.memberIndexMap.end())return false;//键索引重复
            structInfo.memberIndexMap[member.keyIndex]=&member;

            //成员定义结束
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,";"))return false;//语法错误

            return true;
        }

        //处理字典成员
        bool AKStruct::handleDict(const int8 *structStr,uint32 &pos,Struct &structInfo)
        {
            Word word;
            std::string typeName;//数组类型

            //数组类型开始
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,"<"))return false;//语法错误

            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type==Word::OPERATOR)//基本类型数组
            {
                uint32 type=getBaseType(word.val);
                if(type>=1&&type<=30)//数值或字符串
                {
                    typeName=word.val;
                }
                else//基础类型错误
                {
                    return false;
                }
            }
            else if(word.type==Word::SIGN_NAME)//结构体数据
            {
                StructMap::iterator it=structMap.find(word.val);
                if(it==structMap.end())return false;//结构体不存在
                typeName=word.val;
            }

            //数组类型结束
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,">"))return false;//语法错误

            //成员名称
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::SIGN_NAME)return false;//语法错误
            std::string memberName=word.val;
            uint32 type=getBaseType(word.val);
            if(structInfo.memberMap.find(memberName)!=structInfo.memberMap.end())return false;//成员名称重复
            Member &member=structInfo.memberMap[memberName];
            member.name=memberName;
            member.type=41;
            member.val=typeName;

            //键索引开始
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,":"))return false;//语法错误

            //键索引值
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(word.type!=Word::NUMBER)return false;//语法错误
            member.keyIndex=atoi(word.val.c_str());
            if(structInfo.memberIndexMap.find(member.keyIndex)!=structInfo.memberIndexMap.end())return false;//键索引重复
            structInfo.memberIndexMap[member.keyIndex]=&member;

            //成员定义结束
            if(getWord(structStr,pos,word)!=1)return false;//成员定义不完整
            if(!isOperator(word,";"))return false;//语法错误

            return true;
        }

        //获取基础类型ID，失败返回0
        uint32 AKStruct::getBaseType(std::string typeName)
        {
            TypeMap::iterator it=typeMap.find(typeName);
            if(it==typeMap.end())return 0;//查找失败
            return it->second;
        }

        //加载结构体字符串
        bool AKStruct::loadStruct(const int8 *structStr,uint32 &pos)
        {
            int32 rt;
            //uint32 pos=0;
            Word word;
            while(true)
            {
                //结构体定义开始
                if((rt=getWord(structStr,pos,word))==1)//分词成功
                {
                    if(isOperator(word,"struct"))
                    {
                        //结构体名称
                        if((rt=getWord(structStr,pos,word))!=1)return false;////结构体不完整
                        if(word.type!=Word::SIGN_NAME)return false;//语法错误
                        std::string structName=word.val;
                        if(structMap.find(structName)!=structMap.end())return false;//结构体名称重复
                        Struct &structInfo=structMap[structName];
                        //结构体内容开始
                        if((rt=getWord(structStr,pos,word))!=1)return false;//结构体不完整
                        if(!isOperator(word,"{"))return false;//语法错误
                        ///////////////////////////////////////////////////////////////////////////////
                        //成员定义
                        while(true)
                        {
                            if((rt=getWord(structStr,pos,word))!=1)return false;//结构体不完整
                            if(isOperator(word,"}"))//结构体内容结束
                            {
                                break;
                            }
                            else if(word.type==Word::SIGN_NAME)//object结构体成员变量
                            {
                                StructMap::iterator it=structMap.find(word.val);
                                if(it==structMap.end())return false;//结构体不存在
                                if(!handleObject(structStr,pos,structInfo,word.val))return false;
                            }
                            else if(word.type==Word::OPERATOR)
                            {
                                uint32 type=getBaseType(word.val);
                                if(type==0)//语法错误
                                {
                                    return false;
                                }
                                else if(type<=30)//数值或字符串
                                {
                                    if(!handleValMember(structStr,pos,structInfo,type))return false;
                                }
                                else if(type==40)//array数组
                                {
                                    if(!handleArray(structStr,pos,structInfo))return false;
                                }
                                else if(type==41)//dict字典
                                {
                                    if(!handleDict(structStr,pos,structInfo))return false;
                                }
                                else//语法错误
                                {
                                    return false;
                                }
                            }
                            else//语法错误
                            {
                                return false;
                            }
                        }
                        ///////////////////////////////////////////////////////////////////////////////
                    }
                    else if(isOperator(word,";"))
                    {
                        //忽略
                    }
                }
                else if(rt==0)//没有分词了
                {
                    break;
                }
                else//分词失败
                {
                    return false;
                }
            }
            return true;
        }

        //分词,返回:-1=错误;0=没有分词;1=成功;
        int32 AKStruct::getWord(const int8 *code,uint32 &pos,Word &word)
        {
            int8 ch;
            std::string strWord;
            int8 strTmp[3];
            memset(strTmp,0,3);
            for(uint32 i=pos;(ch=code[i])!='\0';++i)
            {
                //跳过无意义字符
                if(inStr(ch," \t\r\n"))
                {
                    ++pos;
                    continue;
                }

                //跳过注释
                if(ch=='/')
                {
                    ch=code[i+1];
                    switch(ch)
                    {
                        case '/'://行注释
                        {
                            for(i+=2;;++i)
                            {
                                ch=code[i];

                                //源代码结束
                                if(ch=='\0')
                                {
                                    pos=i;
                                    return 0;
                                }

                                //注释结束
                                if(inStr(ch,"\r\n"))
                                {
                                    pos=i+1;
                                    break;
                                }
                            }

                            continue;

                            break;
                        }

                        case '*'://段注释
                        {
                            for(i+=2;;++i)
                            {
                                ch=code[i];

                                //源代码结束
                                if(ch=='\0')
                                {
                                    pos=i;
                                    //m_strError="段注释未结束出错";
                                    return -1;
                                }

                                //注释结束
                                if(ch=='*'&&code[i+1]=='/')
                                {
                                    ++i;
                                    pos=i+1;
                                    break;
                                }
                            }

                            continue;

                            break;
                        }

                        default:
                        {
                            ch='/';
                            break;
                        }

                    }
                }

                //双引号开头
                if(ch=='"')
                {
                    pos=i;
                    for(++i;(ch=code[i])!='\0';++i)
                    {
                        //正常结束
                        if(ch=='"')
                        {
                            pos=i+1;
                            word.type=Word::STRING;
                            word.val=strWord;
                            return 1;
                        }

                        //字符串未结束出错
                        if(ch=='\0')
                        {
                            //分词失败
                            //m_strError="字符串未结束出错";
                            return -1;
                        }

                        //遇到转义字符
                        if(ch=='\\')
                        {
                            //正确转义
                            if(code[i+1]!='\0')
                            {
                                ++i;
                                ch=code[i];
                                switch(ch)
                                {
                                    case '\"':
                                    {
                                        strWord+="\"";
                                        break;
                                    }
                                    case '\\':
                                    {
                                        strWord+="\\";
                                        break;
                                    }
                                    case 't':
                                    {
                                        strWord+="\t";
                                        break;
                                    }
                                    case 'r':
                                    {
                                        strWord+="\r";
                                        break;
                                    }
                                    case 'n':
                                    {
                                        strWord+="\n";
                                        break;
                                    }
                                    default:
                                    {
                                        if(ch=='_'||isLetter(ch)||isNumber(ch))
                                        {
                                            strTmp[0]=ch;
                                            strTmp[1]='\0';
                                            strWord+=strTmp;
                                        }
                                        else if(((uint8)ch)>=161&&((uint8)code[i+1])>=161)
                                        {
                                            //汉字
                                            strTmp[0]=ch;
                                            strTmp[1]=code[i+1];
                                            strTmp[2]='\0';
                                            strWord+=(int8 *)strTmp;
                                            ++i;
                                        }
                                        break;
                                    }
                                }
                            }
                            else//字符串未结束出错
                            {
                                //分词失败
                                //m_strError="字符串未结束出错";
                                return -1;
                            }
                        }
                        else//其它字符
                        {
                            //字符串未结束出错
                            if(ch=='\0')
                            {
                                //分词失败
                                //m_strError="字符串未结束出错";
                                return -1;
                            }
                            else
                            {
                                strTmp[0]=ch;
                                strTmp[1]='\0';
                                strWord+=strTmp;
                            }
                        }

                    }
                }

                //以正负号或数字字符开头
                if(
                        (ch=='+'&&isNumber(code[i+1]))||
                        (ch=='-'&&isNumber(code[i+1]))||
                        isNumber(ch)
                        )
                {
                    pos=i;
                    strTmp[0]=ch;
                    strTmp[1]='\0';
                    strWord+=(int8 *)strTmp;
                    ++i;
                    for(;(ch=code[i])!='\0'&&isNumber(ch);++i)
                    {
                        strTmp[0]=ch;
                        strTmp[1]='\0';
                        strWord+=(int8 *)strTmp;
                    }

                    //遇到小数点
                    if(ch=='.')
                    {
                        strTmp[0]=ch;
                        strTmp[1]='\0';
                        strWord+=(int8 *)strTmp;
                        for(++i;(ch=code[i])!='\0'&&isNumber(ch);++i)
                        {
                            strTmp[0]=ch;
                            strWord+=(int8 *)strTmp;
                        }
                    }
                    pos=i;
                    word.type=Word::NUMBER;
                    word.val=strWord;
                    return 1;
                }

                //以下划线/字母/汉字开头
                if(ch=='_'||isLetter(ch)||((uint8)ch)>=161)
                {
                    pos=i;
                    if(((uint8)ch)>=161&&((uint8)code[i+1])>=161)
                    {
                        //汉字
                        strTmp[0]=ch;
                        strTmp[1]=code[i+1];
                        strTmp[2]='\0';
                        strWord+=(int8 *)strTmp;
                        ++i;
                    }
                    else
                    {
                        strTmp[0]=ch;
                        strTmp[1]='\0';
                        strWord+=(int8 *)strTmp;
                    }
                    for(++i;(ch=code[i])!='\0';++i)
                    {
                        if(ch=='_'||isLetter(ch)||isNumber(ch))
                        {
                            //字符或数字
                            strTmp[0]=ch;
                            strTmp[1]='\0';
                            strWord+=strTmp;
                        }
                        else if(((uint8)ch)>=161&&((uint8)code[i+1])>=161)
                        {
                            //汉字
                            strTmp[0]=ch;
                            strTmp[1]=code[i+1];
                            strTmp[2]='\0';
                            strWord+=(int8 *)strTmp;
                            ++i;
                        }
                        else break;
                    }
                    pos=i;

                    if(isOperator((int8 *)strWord.c_str()))
                    {
                        //运算符
                        word.type=Word::OPERATOR;
                        word.val=strWord;
                    }
                    else
                    {
                        //符号名
                        word.type=Word::SIGN_NAME;
                        word.val=strWord;
                    }
                    return 1;
                }

                //判断是否运算符
                pos=i;
                std::list<std::string>::reverse_iterator itOperatorList=m_OperatorList.rbegin();
                while(itOperatorList!=m_OperatorList.rend())
                {
                    const int8 *pOperator=(*itOperatorList).c_str();
                    if(memcmp(pOperator,&code[i],strlen(pOperator))==0)
                    {
                        //关键字匹配成功
                        pos+=strlen(pOperator);
                        word.type=Word::OPERATOR;
                        word.val=pOperator;
                        return 1;
                    }
                    ++itOperatorList;
                }

                //分词失败
                //m_strError="分词失败";
                return -1;
            }
            return 0;//没有分词了
        }

        void AKStruct::changeVarTypeToString(AKVar *var,const int8 *initVal)
        {
            if(var->getType()==AKVar::typeString)return;
            if(var->getType()==AKVar::typeNumber)
            {
                std::string str=var->toStdString();
                var->clearType();
                var->setType(AKVar::typeString);
                var->setValue(str.c_str());
            }
            else
            {
                var->clearType();
                var->setType(AKVar::typeString);
                var->setValue(initVal);
            }
        }

        void AKStruct::changeVarTypeToNumber(AKVar *var,double initVal)
        {
            if(var->getType()==AKVar::typeNumber)return;
            if(var->getType()==AKVar::typeString)
            {
                double num=var->toNumber();
                var->clearType();
                var->setType(AKVar::typeNumber);
                var->setValue(num);
            }
            else
            {
                var->clearType();
                var->setType(AKVar::typeNumber);
                var->setValue(initVal);
            }
        }

        //根据结构体定义升级obj成员
        void AKStruct::upgradeObj(
                AKCore *core,//
                AKVar *var,//需要升级的变量名
                const int8 *structName//结构体名称
        )
        {
            StructMap::iterator it=structMap.find(structName);
            if(it==structMap.end())
            {
                throw "upgrade struct obj error:struct not exist";
            }

            container::AKDict *dict=NULL;
            if(var->getType()!=AKVar::typeHandle)
            {
                var->clearType();
                var->setType(AKVar::typeHandle);
                dict=core->createDict();
                var->setValue((void *)dict);
            }
            else
            {
                dict=(container::AKDict *)var->toHandle();
                if(!core->checkDict(dict))
                {
                    dict=core->createDict();
                    var->setValue((void *)dict);
                    //throw "upgrade struct obj error:obj not as dict";
                }
            }

            Struct &structInfo=it->second;
            Struct::MemberMap &memberMap=structInfo.memberMap;
            Struct::MemberMap::iterator it2=memberMap.begin();
            while(it2!=memberMap.end())
            {
                const std::string &memberName=it2->first;
                Member &member=it2->second;
                AKVar *memberVar=dict->get(memberName.c_str());
                if(memberVar)//成员存在
                {
                    //类型:1=int8;2=uint8;3=int16;4=uint16;5=int32;6=uint32;7=int64;8=uint64;20=float;21=double;30=string;40=array;41=dict;50=object;
                    if(member.type<30)//数值
                    {
                        if(memberVar->getType()!=AKVar::typeNumber)
                        {
                            changeVarTypeToNumber(memberVar,atof(member.val.c_str()));
                        }
                    }
                    else if(member.type==30)//字符串
                    {
                        changeVarTypeToString(memberVar,member.val.c_str());
                    }
                    else if(member.type==40)//array
                    {
                        bool isArr=false;
                        if(memberVar->getType()==AKVar::typeHandle)
                        {
                            AKArr *array=(AKArr *)memberVar->toHandle();
                            if(core->checkArr(array))
                            {
                                isArr=true;
                                uint32 type=getBaseType(member.val);
                                if(type==0)//元素类型为结构体
                                {
                                    AKVarArray &arr=array->mVarArray;
                                    uint32 c=arr.size();
                                    for(uint32 i=0;i<c;++i)
                                    {
                                        upgradeObj(core,arr[i],member.val.c_str());
                                    }
                                }
                                else if(type==30)//元素类型为字符串
                                {
                                    AKVarArray &arr=array->mVarArray;
                                    uint32 c=arr.size();
                                    for(uint32 i=0;i<c;++i)
                                    {
                                        changeVarTypeToString(arr[i],"");
                                    }
                                }
                                else//元素类型为数值
                                {
                                    AKVarArray &arr=array->mVarArray;
                                    uint32 c=arr.size();
                                    for(uint32 i=0;i<c;++i)
                                    {
                                        changeVarTypeToNumber(arr[i],0);
                                    }
                                }
                            }
                        }
                        if(!isArr)
                        {
                            memberVar->clearType();
                            memberVar->setType(AKVar::typeHandle);
                            memberVar->setValue((void *)core->createArr());
                        }
                    }
                    else if(member.type==41)//dict
                    {
                        bool isDict=false;
                        if(memberVar->getType()==AKVar::typeHandle)
                        {
                            AKDict *dict=(AKDict *)memberVar->toHandle();
                            if(core->checkDict(dict))
                            {
                                isDict=true;
                                uint32 type=getBaseType(member.val);
                                if(type==0)//元素类型为结构体
                                {
                                    {
                                        AKUint32VarMap &map=dict->mUint32VarMap;
                                        AKUint32VarMap::iterator it=map.begin();
                                        while(it!=map.end())
                                        {
                                            upgradeObj(core,it->second,member.val.c_str());
                                            ++it;
                                        }
                                    }
                                    {
                                        AKVarMap &map=dict->mVarMap;
                                        AKVarMap::iterator it=map.begin();
                                        while(it!=map.end())
                                        {
                                            upgradeObj(core,it->second,member.val.c_str());
                                            ++it;
                                        }
                                    }
                                }
                                else if(type==30)//元素类型为字符串
                                {
                                    {
                                        AKUint32VarMap &map=dict->mUint32VarMap;
                                        AKUint32VarMap::iterator it=map.begin();
                                        while(it!=map.end())
                                        {
                                            changeVarTypeToString(it->second,"");
                                            ++it;
                                        }
                                    }
                                    {
                                        AKVarMap &map=dict->mVarMap;
                                        AKVarMap::iterator it=map.begin();
                                        while(it!=map.end())
                                        {
                                            changeVarTypeToString(it->second,"");
                                            ++it;
                                        }
                                    }
                                }
                                else//元素类型为数值
                                {
                                    {
                                        AKUint32VarMap &map=dict->mUint32VarMap;
                                        AKUint32VarMap::iterator it=map.begin();
                                        while(it!=map.end())
                                        {
                                            changeVarTypeToNumber(it->second,0);
                                            ++it;
                                        }
                                    }
                                    {
                                        AKVarMap &map=dict->mVarMap;
                                        AKVarMap::iterator it=map.begin();
                                        while(it!=map.end())
                                        {
                                            changeVarTypeToNumber(it->second,0);
                                            ++it;
                                        }
                                    }
                                }
                            }
                        }
                        if(!isDict)
                        {
                            memberVar->clearType();
                            memberVar->setType(AKVar::typeHandle);
                            memberVar->setValue((void *)core->createDict());
                        }
                    }
                    else if(member.type==50)//object
                    {
                        bool isDict=false;
                        if(memberVar->getType()==AKVar::typeHandle)
                        {
                            AKDict *dict=(AKDict *)memberVar->toHandle();
                            if(core->checkDict(dict))
                            {
                                isDict=true;
                                upgradeObj(core,memberVar,member.val.c_str());
                            }
                        }
                        if(!isDict)
                        {
                            memberVar->clearType();
                            memberVar->setType(AKVar::typeHandle);
                            memberVar->setValue((void *)core->createDict());
                            upgradeObj(core,memberVar,member.val.c_str());
                        }
                    }
                }
                else//成员不存在
                {
                    //类型:1=int8;2=uint8;3=int16;4=uint16;5=int32;6=uint32;7=int64;8=uint64;20=float;21=double;30=string;40=array;41=dict;50=object;
                    if(member.type<30)//数值
                    {
                        AKVar *var=new AKVar(core);
                        var->setType(AKVar::typeNumber);
                        var->setValue(atof(member.val.c_str()));
                        dict->set(memberName.c_str(),var);
                    }
                    else if(member.type==30)//字符串
                    {
                        AKVar *var=new AKVar(core);
                        var->setType(AKVar::typeString);
                        var->setValue(member.val.c_str());
                        dict->set(memberName.c_str(),var);
                    }
                    else if(member.type==40)//array
                    {
                        AKVar *var=new AKVar(core);
                        var->setType(AKVar::typeHandle);
                        var->setValue((void *)core->createArr());
                        dict->set(memberName.c_str(),var);
                    }
                    else if(member.type==41)//dict
                    {
                        AKVar *var=new AKVar(core);
                        var->setType(AKVar::typeHandle);
                        var->setValue((void *)core->createDict());
                        dict->set(memberName.c_str(),var);
                    }
                    else if(member.type==50)//object
                    {
                        AKVar *var=new AKVar(core);
                        var->setType(AKVar::typeHandle);
                        var->setValue((void *)core->createDict());
                        upgradeObj(core,var,member.val.c_str());
                        dict->set(memberName.c_str(),var);
                    }
                }
                ++it2;
            }
        }

        bool AKStruct::inStr(int8 ch,const int8 *str)
        {
            unsigned long i;
            int8 ch2;

            for(i=0;(ch2=str[i])!='\0';++i)
            {
                if(ch==ch2)return true;
            }

            return false;
        }
        bool AKStruct::isLetter(int8 ch)
        {
            if((ch>='a'&&ch<='z')||
               (ch>='A'&&ch<='Z'))
                return true;

            return false;
        }
        bool AKStruct::isNumber(int8 ch)
        {
            if(ch>='0'&&ch<='9')return true;

            return false;
        }
        bool AKStruct::isOperator(const int8 *pWord)
        {
            std::list<std::string>::iterator it=m_OperatorList.begin();
            while(it!=m_OperatorList.end())
            {
                const int8 *pOperator=(*it).c_str();
                if(strcmp(pOperator,pWord)==0)return true;
                ++it;
            }
            return false;
        }
    }
}