#include "ak.h"
#include "AKDataTool.h"

namespace ak
{
    AKDataTool::AKDataTool(AKCore *core)
            :mCore(core),
             mSrcBuf(core),
             mDestBuf(core)
    {
    }

    AKDataTool::~AKDataTool()
    {
    }

    void AKDataTool::set(
            const int8 *data,//数据
            uint32 size//尺寸
    )
    {
        mSrcBuf.clear();
        mSrcBuf.write(data,size);
    }

    ak::container::AKBuf &AKDataTool::get()
    {
        mDestBuf.setPos(0);
        return mDestBuf;
    }

    //4位十六进制数转字母
    int8 AKDataTool::ch16(uint32 num)
    {
        if(num<10)return '0'+num;
        return 'a'+(num-10);
    }

    //4位十六进制字符转16进制数值
    uint32 AKDataTool::num16(
            int8 ch
    )
    {
        if(ch>='0'&&ch<='9')return ch-'0';
        return ch-'a'+10;
    }

    void AKDataTool::encrypt1()
    {
        int8 ch;
        mDestBuf.clear();
        mSrcBuf.setPos(0);
        uint32 c=mSrcBuf.size();
        for(uint32 i=0;i<c;++i)
        {
            mSrcBuf.readInt8(ch);
            uint8 t=ch;
            t=255-t;
            mDestBuf.writeInt8(ch16(t>>4));//写入高4位
            mDestBuf.writeInt8(ch16(t&0xf));//写入低4位
        }
    }

    void AKDataTool::decrypt1()
    {
        int8 h,l;
        mDestBuf.clear();
        mSrcBuf.setPos(0);
        uint32 c=mSrcBuf.size();
        for(uint32 i=0;i<c;i+=2)
        {
            mSrcBuf.readInt8(h);
            mSrcBuf.readInt8(l);
            uint8 t=num16(h)*16+num16(l);
            t=255-t;
            mDestBuf.writeInt8(t);
        }
    }

}
