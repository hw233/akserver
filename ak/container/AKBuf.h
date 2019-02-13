//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKBUF_H
#define AKSERVER_AKBUF_H

#include "AKBufBlockFactory.h"

namespace ak
{
    namespace container
    {
        //缓冲类
        class AKBuf:public AKRef
        {
        public:
            AKBuf(AKCore *core);
            ~AKBuf();

            //获取尺寸
            uint32 size();

            //清空
            void clear();

            //获取位置
            uint32 getPos();

            //设置位置（可能增加缓冲大小）
            void setPos(
                    uint32 pos//位置
            );

            //设置位置（不影响缓冲大小）
            void offset(
                    uint32 pos//位置
            );

            //设置位置到尾部
            void setEnd();

            //写入数据（根据当前位置）
            void write(
                    const int8 *data,//数据
                    uint32 size//尺寸
            );

            //读出数据（根据当前位置）
            bool read(
                    int8 *data,//数据
                    uint32 size//尺寸
            );

            void writeInt8(int8 num);
            void writeInt16(int16 num);
            void writeInt32(int32 num);
            void writeUint8(uint8 num);
            void writeUint16(uint16 num);
            void writeUint32(uint32 num);
            void writeFloat(float num);
            void writeDouble(double num);

            bool readInt8(int8 &num);
            bool readInt16(int16 &num);
            bool readInt32(int32 &num);
            bool readUint8(uint8 &num);
            bool readUint16(uint16 &num);
            bool readUint32(uint32 &num);
            bool readFloat(float &num);
            bool readDouble(double &num);

            //附加数据到当前位置
            void append(
                    AKBuf &buf,//源buf
                    uint32 offset=0,//源buf偏移位置
                    uint32 size=0//需要复制的尺寸（0代表复制最大尺寸）
            );

            //获取块数量
            uint32 getBlockCount();

            //获取块数据
            int8 *getBlockData(
                    uint32 index,//块索引
                    uint32 &blockSize//块数据尺寸
            );

            //删除首部数据块
            void popBlock();

        private:
            void fit();
        private:
            AKBufBlockFactory *mFactory;
            AKBlockArray mBlockArray;
            uint32 mSize;//占用字节数
            uint32 mPos;//当前位置
        };
    }
}

#endif //AKSERVER_AKBUF_H
