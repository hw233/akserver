//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKARR_H
#define AKSERVER_AKARR_H

#include <ak.h>

namespace ak
{
    namespace container
    {
        //脚本数组类
        class AKArr:public AKRef
        {
        public:
            AKArr(AKCore *core);
            ~AKArr();

            uint32 size();
            void clear();

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

            //压入值
            void push(
                    const int8 *val,//值
                    uint32 bytes=0//字符串字节数
            );

            //压入值
            void push(
                    double val//值
            );

            //压入值
            void push(
                    void *handle//句柄值
            );

            //压入值
            void push(
                    AKVar *var//变量
            );

            //通过字符串分割转换成数组
            void fromSplitString(
                    const int8 *str,//源字符串
                    const int8 *spliter//分割字符串
            );

            //根据值查找并返回字符串键
            bool findStringKey(
                    std::string &key,//键
                    AKVar *val//值
            );

            //根据值查找并返回uint32键
            bool findUint32Key(
                    uint32 &key,//键
                    AKVar *val//值
            );

            //void addChild(AKArr *arr);//添加子数组（父数组释放时同时释放所有子数组）

        public:
            AKVarArray mVarArray;//数值索引表
            AKVarMap mVarMap;//字符串索引表

            //private:
            //	AKArr *mParent;//父
            //	AKArrMap mChildren;//子
        };
    }
}

#endif //AKSERVER_AKARR_H
