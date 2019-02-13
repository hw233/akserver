//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKDICT_H
#define AKSERVER_AKDICT_H

#include <ak.h>

namespace ak
{
    namespace container
    {
        class AKDict;

        //脚本字典迭代器
        class AKDictIterator
        {
        public:
            friend class AKDict;
            AKDictIterator();
            bool operator==(AKDictIterator it);//比较相等
            void operator++();//下一元素
            void operator--();//下一元素
            operator bool();//是否为空
            AKVar *val();//返回值
            uint32 keyUint32();//返回整数键
            const int8 *keyString();//返回字符串键
            AKDict *getDict();
        private:
            AKDict *dict;
            uint32 witch;//哪个迭代器起作用:1=mUint32VarMap;2=mVarMap;
            AKUint32VarMap::iterator it1;
            AKVarMap::iterator it2;
        };

        //脚本字典类
        class AKDict:public AKRef
        {
        public:
            AKDict(AKCore *core);
            ~AKDict();

            uint32 size();
            void clear();

            AKDictIterator begin();//返回开始迭代器
            AKDictIterator end();//返回结束迭代器

            //删除
            void remove(
                    const int8 *index//字符串索引
            );

            //删除
            void remove(
                    uint32 index//数值索引
            );

            //设置值
            void set(
                    const int8 *index,//字符串索引
                    const int8 *val,//字符串值
                    uint32 bytes=0//字符串字节数
            );

            //设置值
            void set(
                    const int8 *index,//字符串索引
                    double val//数值
            );

            //设置值
            void set(
                    const int8 *index,//字符串索引
                    void *handle//句柄
            );

            //设置值
            void set(
                    const int8 *index,//字符串索引
                    AKVar *var//变量
            );

            //设置值
            void set(
                    uint32 index,//数值索引
                    const int8 *val,//字符串值
                    uint32 bytes=0//字符串字节数
            );

            //设置值
            void set(
                    uint32 index,//数值索引
                    double val//数值
            );

            //设置值
            void set(
                    uint32 index,//数值索引
                    void *handle//句柄
            );

            //设置值
            void set(
                    uint32 index,//数值索引
                    AKVar *var//变量
            );

            //获取值
            AKVar *get(
                    const int8 *index//字符串索引
            );

            //获取值
            AKVar *get(
                    uint32 index//数值索引
            );

        public:
            AKUint32VarMap mUint32VarMap;//数值索引表
            AKVarMap mVarMap;//字符串索引表
        };
    }
}

#endif //AKSERVER_AKDICT_H
