#include "ak.h"
#include "AKUseMutex.h"

namespace ak
{
    AKUseMutex::AKUseMutex(
            boost::recursive_mutex &mutex
    )
    {
        m_recursive_mutex=&mutex;
        m_recursive_mutex->lock();
    }

    AKUseMutex::~AKUseMutex()
    {
        if(m_recursive_mutex)
        {
            m_recursive_mutex->unlock();
        }
    }
}
