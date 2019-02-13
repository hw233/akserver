#include "ak.h"
#include "AKStaticMem.h"

namespace ak
{
    namespace container
    {
        AKStaticMem::AKStaticMem(
                uint32 size//静态内存空间尺寸
        )
                :mSize(size)
        {
            mMem=new int8[mSize];
        }

        AKStaticMem::~AKStaticMem()
        {
            delete [] mMem;
        }

        int8 *AKStaticMem::get()
        {
            return mMem;
        }

        uint32 AKStaticMem::size()
        {
            return mSize;
        }

        AKUseStaticMem::AKUseStaticMem(
                AKStaticMem &mem//静态内存对象
        )
                :mMem(mem.get()),
                 mMemSize(mem.size()),
                 mNewMem(NULL),
                 mNewMemSize(0)
        {
        }

        AKUseStaticMem::~AKUseStaticMem()
        {
            if(mNewMem)delete [] mNewMem;
        }

        int8 *AKUseStaticMem::alloc(
                uint32 size//所需尺寸
        )
        {
            if(size<=mMemSize)return mMem;
            if(mNewMem)
            {
                if(size<=mNewMemSize)return mNewMem;
                delete [] mNewMem;
            }
            mNewMemSize=size;
            mNewMem=new int8[mNewMemSize];
            return mNewMem;
        }

    }
}
