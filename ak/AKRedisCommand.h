//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKREDISCOMMAND_H
#define AKSERVER_AKREDISCOMMAND_H

namespace ak
{
    //
    class AKRedisCommand:virtual public acl::redis_command
    {
    public:
        AKRedisCommand();
        ~AKRedisCommand();

        bool setexnx(const char* key, const char* value, int timeout);

        bool setexnx(
                const char* key, size_t key_len,
                const char* value,size_t value_len, int timeout
        );

        bool psetexnx(const char* key, const char* value, int timeout);

        bool psetexnx(
                const char* key, size_t key_len,
                const char* value,size_t value_len, int timeout
        );
    };
}

#endif //AKSERVER_AKREDISCOMMAND_H
