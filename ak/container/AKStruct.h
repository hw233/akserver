//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSTRUCT_H
#define AKSERVER_AKSTRUCT_H

namespace ak
{
    namespace container
    {
        //AK结构体类
        class AKStruct
        {
        public:
            //成员信息
            struct Member
            {
                Member():
                        keyIndex(0),
                        type(0)
                {
                }
                std::string name;
                uint32 keyIndex;//键索引
                uint32 type;//类型:1=int8;2=uint8;3=int16;4=uint16;5=int32;6=uint32;7=int64;8=uint64;20=float;21=double;30=string;40=array;41=dict;50=object;
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                std::string val;
                /*
                type>=1&&type<=30时：代表数值或字符串默认值
                type>30时：代表各种容器的元素类型
                */
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            };
            //结构信息
            struct Struct
            {
                typedef std::map<std::string,Member> MemberMap;
                typedef std::map<uint32,Member *> MemberIndexMap;
                MemberMap memberMap;//成员列表
                MemberIndexMap memberIndexMap;//成员键索引列表
            };
            typedef std::map<std::string,Struct> StructMap;//结构体列表
            //单词
            struct Word
            {
                enum TYPE
                {
                    NO_INIT=0,//未初始化
                    SIGN_NAME,//符号名称(例如:变量名,函数名)
                    OPERATOR,//运算符(+-*/...)
                    NUMBER,//数值
                    STRING,//字符串
                };
                TYPE type;//类型
                std::string val;//值
            };
            AKStruct();

            //是否为指定运算符
            bool isOperator(Word &word,const int8 *name);

            //处理值类型成员，类型:1=int8;2=uint8;3=int16;4=uint16;5=int32;6=uint32;7=int64;8=uint64;20=float;21=double;30=string;
            bool handleValMember(const int8 *structStr,uint32 &pos,Struct &structInfo,uint32 memberType);

            //处理结构体对象成员
            bool handleObject(const int8 *structStr,uint32 &pos,Struct &structInfo,std::string structName);

            //处理数组成员
            bool handleArray(const int8 *structStr,uint32 &pos,Struct &structInfo);

            //处理字典成员
            bool handleDict(const int8 *structStr,uint32 &pos,Struct &structInfo);

            //获取基础类型ID，失败返回0
            uint32 getBaseType(std::string typeName);

            //加载结构体字符串
            bool loadStruct(const int8 *structStr,uint32 &pos);

            //分词,返回:-1=错误;0=没有分词;1=成功;
            int32 getWord(const int8 *code,uint32 &pos,Word &word);

            //根据结构体定义升级obj成员
            void upgradeObj(
                    AKCore *core,//
                    AKVar *var,//需要升级的变量名
                    const int8 *structName//结构体名称
            );

        private:
            void changeVarTypeToString(AKVar *var,const int8 *initVal);
            void changeVarTypeToNumber(AKVar *var,double initVal);
            bool inStr(int8 ch,const int8 *str);
            bool isLetter(int8 ch);
            bool isNumber(int8 ch);
            bool isOperator(const int8 *pWord);

        private:
            typedef std::list<std::string> StrList;
            StrList m_OperatorList;//运算符列表
            typedef std::map<std::string,uint32> TypeMap;
            TypeMap typeMap;
        public:
            StructMap structMap;
        };
    }
}

#endif //AKSERVER_AKSTRUCT_H
