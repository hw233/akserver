#include "ak.h"
#include "AKBufBlockFactory.h"

namespace ak
{
    namespace container
    {
        AKBufBlockFactory::AKBufBlockFactory()
        {
        }

        AKBufBlockFactory::~AKBufBlockFactory()
        {
            {
                AKBlockList::iterator it=mBlockList.begin();
                while(it!=mBlockList.end())
                {
                    delete *it;
                    ++it;
                }
            }
        }

        int8 *AKBufBlockFactory::alloc(
                bool isNeedClean
        )
        {
            int8 *block=NULL;
            mBlockListMutex.lock();
            if(mBlockList.size()==0)
            {
                block=new int8[AKBUF_BLOCK_SIZE];
            }
            else
            {
                block=*mBlockList.begin();
                mBlockList.pop_front();
            }
            mBlockListMutex.unlock();
            if(isNeedClean)memset(block,0,AKBUF_BLOCK_SIZE);
            return block;
        }

        void AKBufBlockFactory::free(int8 *block)
        {
            mBlockListMutex.lock();
            mBlockList.push_back(block);
            mBlockListMutex.unlock();
        }
    }
}
