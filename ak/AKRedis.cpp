#include "ak.h"
#include "AKRedis.h"

namespace ak
{
    AKRedis::AKRedis(
            const int8 *domainPort,//域名端口
            uint32 connectTimeout,//连接超时（毫秒）
            uint32 readWriteTimeout,//读写超时（毫秒）
            const int8 *password
    ):client(domainPort,connectTimeout,readWriteTimeout)
    {
        if(strcmp(password,"")!=0)
        {
            client.set_password(password);
        }
        client.set_slice_request(false);
        hash.set_client(&client);
        list.set_client(&client);
        key.set_client(&client);
        string.set_client(&client);
        set.set_client(&client);
        zset.set_client(&client);
        server.set_client(&client);
        akcommand.set_client(&client);
    }
    AKRedis::~AKRedis()
    {
    }
}