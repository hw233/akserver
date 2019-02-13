//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKDATATOOL_H
#define AKSERVER_AKDATATOOL_H

namespace ak
{
    //数据工具类
    class AKDataTool
    {
    public:
        AKDataTool(AKCore *core);
        ~AKDataTool();

        //设置数据
        void set(
                const int8 *data,//数据
                uint32 size//尺寸
        );

        //获取数据
        ak::container::AKBuf &get();

        void encrypt1();//加密1
        void decrypt1();//解密1

        //void encrypt2();//加密2
        //void decrypt2();//解密2

    private:

        //4位十六进制数转字符
        int8 ch16(
                uint32 num//4位十六进制数
        );

        //4位十六进制字符转16进制数值
        uint32 num16(
                int8 ch//4位十六进制字符
        );

    private:
        AKCore *mCore;
        ak::container::AKBuf mSrcBuf;//源数据缓冲
        ak::container::AKBuf mDestBuf;//目标数据缓冲
    };
}


#endif //AKSERVER_AKDATATOOL_H
