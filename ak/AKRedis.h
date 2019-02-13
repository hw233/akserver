//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKREDIS_H
#define AKSERVER_AKREDIS_H

#include "AKRedisCommand.h"

namespace ak
{
    //Redis封装类
    class AKRedis
    {
    public:
        AKRedis(
                const int8 *domainPort,//域名端口
                uint32 connectTimeout,//连接超时（毫秒）
                uint32 readWriteTimeout,//读写超时（毫秒）
                const int8 *password
        );
        ~AKRedis();
    public:
        acl::redis_client client;//redis客户端对象
        acl::redis_hash hash;//hash操作接口对象
        acl::redis_list list;//list操作接口对象
        acl::redis_key key;//key操作接口对象
        acl::redis_string string;//string操作接口对象
        acl::redis_set set;//set操作接口对象
        acl::redis_zset zset;//zset操作接口对象
        acl::redis_server server;//set操作接口对象
        AKRedisCommand akcommand;
    };
}


#endif //AKSERVER_AKREDIS_H
