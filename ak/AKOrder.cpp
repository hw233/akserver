#include "ak.h"
#include "AKOrder.h"

namespace ak
{
    AKOrder::AKOrder()
            :func(NULL),
             scriptFuncVar(NULL)
    {
    }

    AKOrder::~AKOrder()
    {
    }

    AKVar *AKOrder::getParam(uint32 index)
    {
        if(varParams.size()<=index)return NULL;
        return varParams[index];
    }
}
