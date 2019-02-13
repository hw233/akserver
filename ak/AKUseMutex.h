//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKUSEMUTEX_H
#define AKSERVER_AKUSEMUTEX_H

namespace ak
{
    //锁使用类
    class AKUseMutex
    {
    public:
        AKUseMutex(
                boost::recursive_mutex &mutex
        );
        ~AKUseMutex();

    private:
        boost::recursive_mutex *m_recursive_mutex;//递归锁
    };
}

#endif //AKSERVER_AKUSEMUTEX_H
