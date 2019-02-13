#include "ak.h"
#include "AKRedisCommand.h"

namespace ak
{
    AKRedisCommand::AKRedisCommand()
    {
    }

    AKRedisCommand::~AKRedisCommand()
    {
    }

    bool AKRedisCommand::psetexnx(const char* key, const char* value, int timeout)
    {
        return psetexnx(key, strlen(key), value, strlen(value), timeout);
    }

    bool AKRedisCommand::psetexnx(
            const char* key, size_t key_len,
            const char* value,size_t value_len,
            int timeout
    )
    {
        const char* argv[6];
        size_t lens[6];

        argv[0] = "SET";
        lens[0] = sizeof("SET") - 1;

        argv[1] = key;
        lens[1] = key_len;

        argv[2] = value;
        lens[2] = value_len;

        argv[3] = "PX";
        lens[3] = sizeof("PX") - 1;

        char buf[11];
        sprintf(buf,"%d",timeout);
        argv[4] = buf;
        lens[4] = strlen(buf);

        argv[5] = "NX";
        lens[5] = sizeof("NX") - 1;

        hash_slot(key, key_len);
        build_request(6, argv, lens);
        return check_status();
    }

    bool AKRedisCommand::setexnx(const char* key, const char* value, int timeout)
    {
        return setexnx(key, strlen(key), value, strlen(value), timeout);
    }

    bool AKRedisCommand::setexnx(
            const char* key, size_t key_len,
            const char* value,size_t value_len,
            int timeout
    )
    {
        const char* argv[6];
        size_t lens[6];

        argv[0] = "SET";
        lens[0] = sizeof("SET") - 1;

        argv[1] = key;
        lens[1] = key_len;

        argv[2] = value;
        lens[2] = value_len;

        argv[3] = "EX";
        lens[3] = sizeof("EX") - 1;

        char buf[11];
        sprintf(buf,"%d",timeout);
        argv[4] = buf;
        lens[4] = strlen(buf);

        argv[5] = "NX";
        lens[5] = sizeof("NX") - 1;

        hash_slot(key, key_len);
        build_request(6, argv, lens);
        return check_status();
    }
}
