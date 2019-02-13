//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKBUFBLOCKFACTORY_H
#define AKSERVER_AKBUFBLOCKFACTORY_H

#include <ak.h>

#define AKBUF_BLOCK_SIZE 512

namespace ak
{
    namespace container
    {
        class AKBuf;

        //缓冲块工厂类
        class AKBufBlockFactory
        {
        public:
            friend class AKBuf;
        private:
            AKBufBlockFactory();
            ~AKBufBlockFactory();

            AKDefineSingleton(AKBufBlockFactory);
            int8 *alloc(//分配内存块
                    bool isNeedClean=false//是否需要memset
            );
            void free(int8 *block);//回收内存块

        private:
            AKBlockList mBlockList;
            boost::recursive_mutex mBlockListMutex;
        };
    }
}

#endif //AKSERVER_AKBUFBLOCKFACTORY_H
