#include "ak.h"
#include "AKBuf.h"

namespace ak
{
    namespace container
    {
        AKBuf::AKBuf(AKCore *core)
                :AKRef(core),
                 mFactory(&AKBufBlockFactory::getSingleton()),
                 mPos(0),
                 mSize(0)
        {
        }

        AKBuf::~AKBuf()
        {
            AKBlockArray::iterator it=mBlockArray.begin();
            while(it!=mBlockArray.end())
            {
                mFactory->free(*it);
                ++it;
            }
        }

        uint32 AKBuf::size()
        {
            return mSize;
        }

        void AKBuf::clear()
        {
            mPos=0;
            mSize=0;
            fit();
        }

        void AKBuf::append(AKBuf &buf,uint32 offset,uint32 size)
        {
            if(offset>=buf.size())return;
            uint32 maxSize=buf.size()-offset;//最大可复制尺寸（从buf的offset到尾部的尺寸）
            if(size>maxSize)size=maxSize;//超出buf最大可复制尺寸时，自动截断
            if(size==0)size=maxSize;//0代表最大可复制尺寸

            uint32 blockID=offset/AKBUF_BLOCK_SIZE;
            uint32 posBegin=offset%AKBUF_BLOCK_SIZE;

            uint32 spareSize=size;
            while(spareSize>0)
            {
                int8 *block=buf.mBlockArray[blockID];
                uint32 copySize=AKBUF_BLOCK_SIZE-posBegin;
                if(copySize>spareSize)copySize=spareSize;

                write(&block[posBegin],copySize);
                spareSize-=copySize;

                posBegin=0;
                ++blockID;
            }
        }

        void AKBuf::writeInt8(int8 num)
        {
            write((const int8 *)&num,sizeof(int8));
        }

        void AKBuf::writeInt16(int16 num)
        {
            write((const int8 *)&num,sizeof(int16));
        }

        void AKBuf::writeInt32(int32 num)
        {
            write((const int8 *)&num,sizeof(int32));
        }

        void AKBuf::writeUint8(uint8 num)
        {
            write((const int8 *)&num,sizeof(uint8));
        }

        void AKBuf::writeUint16(uint16 num)
        {
            write((const int8 *)&num,sizeof(uint16));
        }

        void AKBuf::writeUint32(uint32 num)
        {
            write((const int8 *)&num,sizeof(uint32));
        }

        void AKBuf::writeFloat(float num)
        {
            write((const int8 *)&num,sizeof(float));
        }

        void AKBuf::writeDouble(double num)
        {
            write((const int8 *)&num,sizeof(double));
        }

        bool AKBuf::readInt8(int8 &num)
        {
            return read((int8 *)&num,sizeof(int8));
        }

        bool AKBuf::readInt16(int16 &num)
        {
            return read((int8 *)&num,sizeof(int16));
        }

        bool AKBuf::readInt32(int32 &num)
        {
            return read((int8 *)&num,sizeof(int32));
        }

        bool AKBuf::readUint8(uint8 &num)
        {
            return read((int8 *)&num,sizeof(uint8));
        }

        bool AKBuf::readUint16(uint16 &num)
        {
            return read((int8 *)&num,sizeof(uint16));
        }

        bool AKBuf::readUint32(uint32 &num)
        {
            return read((int8 *)&num,sizeof(uint32));
        }

        bool AKBuf::readFloat(float &num)
        {
            return read((int8 *)&num,sizeof(float));
        }

        bool AKBuf::readDouble(double &num)
        {
            return read((int8 *)&num,sizeof(double));
        }

        uint32 AKBuf::getPos()
        {
            return mPos;
        }

        //设置位置到尾部
        void AKBuf::setEnd()
        {
            mPos=mSize;
        }

        //设置位置
        void AKBuf::offset(
                uint32 pos//位置
        )
        {
            if(pos==0&&mSize==0)return;
            mPos=pos;
            if(mPos>=mSize)
            {
                mPos=mSize;
            }
        }

        //设置位置
        void AKBuf::setPos(
                uint32 pos//位置
        )
        {
            if(pos==0&&mSize==0)return;

            mPos=pos;
            if(mPos>=mSize)
            {
                mSize=mPos+1;
                fit();
            }
        }

        //写入数据（根据当前位置）
        void AKBuf::write(
                const int8 *data,//数据
                uint32 size//尺寸
        )
        {
            if(size==0)return;

            uint32 needSize=mPos+size;
            if(needSize>mSize)
            {
                mSize=needSize;
                fit();
            }

            int8 *block;
            uint32 curBlockID=mPos/AKBUF_BLOCK_SIZE;
            uint32 spareSize=size;
            uint32 posBegin=mPos%AKBUF_BLOCK_SIZE;
            while(spareSize>0)
            {
                block=mBlockArray[curBlockID];
                uint32 pieceSize=AKBUF_BLOCK_SIZE-posBegin;

                if(pieceSize<=spareSize)
                {

                    memcpy(&block[posBegin],(void *)&data[size-spareSize],pieceSize);
                    spareSize-=pieceSize;
                }
                else
                {
                    memcpy(&block[posBegin],(void *)&data[size-spareSize],spareSize);
                    break;
                }
                ++curBlockID;
                posBegin=0;
            }
            mPos+=size;
        }

        //读出数据（根据当前位置）
        bool AKBuf::read(
                int8 *data,//数据
                uint32 size//尺寸
        )
        {
            if(size==0)return true;

            uint32 needSize=mPos+size;
            if(needSize>mSize)return false;

            int8 *block;
            uint32 curBlockID=mPos/AKBUF_BLOCK_SIZE;
            uint32 spareSize=size;
            uint32 posBegin=mPos%AKBUF_BLOCK_SIZE;
            while(spareSize>0)
            {
                block=mBlockArray[curBlockID];
                uint32 pieceSize=AKBUF_BLOCK_SIZE-posBegin;
                if(pieceSize<=spareSize)
                {
                    memcpy(&data[size-spareSize],&block[posBegin],pieceSize);
                    spareSize-=pieceSize;
                }
                else
                {
                    memcpy(&data[size-spareSize],&block[posBegin],spareSize);
                    break;
                }
                ++curBlockID;
                posBegin=0;
            }
            mPos+=size;
            return true;
        }

        void AKBuf::fit()
        {
            uint32 needBlockCount=mSize/AKBUF_BLOCK_SIZE;
            if((mSize%AKBUF_BLOCK_SIZE)>0)++needBlockCount;

            uint32 blockCount=mBlockArray.size();
            if(needBlockCount==blockCount)return;

            uint32 i;
            if(needBlockCount>blockCount)
            {
                uint32 newCount=needBlockCount-blockCount;
                for(i=0;i<newCount;++i)
                {
                    mBlockArray.push_back(mFactory->alloc(true));
                }
            }
            else
            {
                AKBlockArray::iterator itDeleteBegin;
                AKBlockArray::iterator it=mBlockArray.begin();
                for(i=0;i<blockCount;++i,++it)
                {
                    if(i==needBlockCount)itDeleteBegin=it;
                    if(i>=needBlockCount)
                    {
                        mFactory->free(*it);
                    }
                }
                mBlockArray.erase(itDeleteBegin,mBlockArray.end());
            }
        }

        uint32 AKBuf::getBlockCount()
        {
            return mBlockArray.size();
        }

        int8 *AKBuf::getBlockData(
                uint32 index,
                uint32 &blockSize
        )
        {
            uint32 blockCount=mBlockArray.size();
            if(index>=blockCount)
            {
                blockSize=0;
                return NULL;
            }

            if(index==(blockCount-1))
            {
                blockSize=mSize%AKBUF_BLOCK_SIZE;
                if(blockSize==0)blockSize=AKBUF_BLOCK_SIZE;
            }
            else
            {
                blockSize=AKBUF_BLOCK_SIZE;
            }
            return mBlockArray[index];
        }

        void AKBuf::popBlock()
        {
            if(mBlockArray.size()==0)return;

            if(mBlockArray.size()==1)
            {
                clear();
            }
            else
            {
                AKBlockArray::iterator it=mBlockArray.begin();
                mFactory->free(*it);
                mBlockArray.erase(it);
                mSize-=AKBUF_BLOCK_SIZE;
                if(mPos<AKBUF_BLOCK_SIZE)
                {
                    mPos=0;
                }
                else
                {
                    mPos-=AKBUF_BLOCK_SIZE;
                }
            }
        }

    }
}
