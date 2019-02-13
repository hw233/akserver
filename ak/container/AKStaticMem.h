//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKSTATICMEM_H
#define AKSERVER_AKSTATICMEM_H

namespace ak
{
    namespace container
    {
#define AKStaticMemSize 1024*64//推荐使用的静态内存空间

        //静态内存类
        class AKStaticMem
        {
        public:
            AKStaticMem(
                    uint32 size=AKStaticMemSize//静态内存空间尺寸
            );
            ~AKStaticMem();

            //获取内存空间
            int8 *get();

            //获取内存空间尺寸
            uint32 size();

        private:
            int8 *mMem;//静态内存空间
            uint32 mSize;//静态内存空间尺寸
        };

        //静态内存使用类
        class AKUseStaticMem
        {
        public:
            AKUseStaticMem(
                    AKStaticMem &mem//静态内存对象
            );
            ~AKUseStaticMem();

            //申请内存空间
            int8 *alloc(
                    uint32 size//所需尺寸
            );

        private:
            int8 *mMem;//静态内存空间
            uint32 mMemSize;//静态内存空间尺寸
            int8 *mNewMem;//动态内存空间
            uint32 mNewMemSize;//动态内存空间尺寸
        };


    }
}

#endif //AKSERVER_AKSTATICMEM_H
