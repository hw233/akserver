//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEFILE_H
#define AKSERVER_AKHANDLEFILE_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;
    class AKHandleDict;

    //文件指令类
    class AKHandleFile:public AKHandler
    {
    public:
        AKHandleFile();
        ~AKHandleFile();
        AKDefineCreate(AKHandleFile);

        virtual void init(AKScript *script);
        virtual void reset();

    public:
        AKDefineScriptFunc(fsize);
        AKDefineScriptFunc(_fopen);
        AKDefineScriptFunc(mypopen);
        AKDefineScriptFunc(fcheck);
        AKDefineScriptFunc(_fclose);
        AKDefineScriptFunc(_fseek);
        AKDefineScriptFunc(_ftell);
        AKDefineScriptFunc(_fgetc);
        AKDefineScriptFunc(mypclose);
        AKDefineScriptFunc(_fread);
        AKDefineScriptFunc(_fwrite);
        AKDefineScriptFunc(log);
        AKDefineScriptFunc(logFromBuf);
        AKDefineScriptFunc(_getline);

        AKDefineScriptFunc(del);
        AKDefineScriptFunc(createDir);
        AKDefineScriptFunc(dirExist);
        AKDefineScriptFunc(fileList);
        AKDefineScriptFunc(fileCopy);
        AKDefineScriptFunc(fileExist);

        AKDefineScriptFunc(redis);
        //AKDefineScriptFunc(redis_select);
        //
        AKDefineScriptFunc(redis_time);
        AKDefineScriptFunc(redis_flushall);
        AKDefineScriptFunc(redis_flushdb);
        //
        AKDefineScriptFunc(redis_del);
        AKDefineScriptFunc(redis_exists);
        //
        AKDefineScriptFunc(redis_set);
        AKDefineScriptFunc(redis_get);
        AKDefineScriptFunc(redis_setnx);
        AKDefineScriptFunc(redis_getset);
        AKDefineScriptFunc(redis_setex);
        AKDefineScriptFunc(redis_psetex);
        AKDefineScriptFunc(redis_setexnx);
        AKDefineScriptFunc(redis_psetexnx);
        //
        AKDefineScriptFunc(redis_hset);
        AKDefineScriptFunc(redis_hget);
        AKDefineScriptFunc(redis_hmset);
        AKDefineScriptFunc(redis_hmget);
        AKDefineScriptFunc(redis_hgetall);
        AKDefineScriptFunc(redis_hdel);
        AKDefineScriptFunc(redis_hexists);
        //
        AKDefineScriptFunc(redis_llen);
        AKDefineScriptFunc(redis_lpush);
        AKDefineScriptFunc(redis_lpushx);
        AKDefineScriptFunc(redis_rpush);
        AKDefineScriptFunc(redis_rpushx);
        AKDefineScriptFunc(redis_lpop);
        AKDefineScriptFunc(redis_rpop);
        AKDefineScriptFunc(redis_lindex);
        AKDefineScriptFunc(redis_lrange);
        AKDefineScriptFunc(redis_lrem);
        //
        AKDefineScriptFunc(redis_sadd);
        AKDefineScriptFunc(redis_scard);
        AKDefineScriptFunc(redis_smembers);
        AKDefineScriptFunc(redis_sismember);
        AKDefineScriptFunc(redis_spop);
        AKDefineScriptFunc(redis_srandmember);
        AKDefineScriptFunc(redis_srem);
        //
        AKDefineScriptFunc(redis_zadd);
        AKDefineScriptFunc(redis_zrem);
        AKDefineScriptFunc(redis_zrange);
        AKDefineScriptFunc(redis_zrevrange);
        AKDefineScriptFunc(redis_zrank);
        AKDefineScriptFunc(redis_zrevrank);
        AKDefineScriptFunc(redis_zcard);

    private:
        AKHandleBuf *getHandleBuf();
        AKHandleDict *getHandleDict();

        FILE *createFile(const int8 *filePath,const int8 *method);
        FILE *createPFile(const int8 *command,const int8 *method);
        bool checkFile(FILE *fp);
        void destroyFile(FILE *fp);
        void destroyPFile(FILE *fp);

        AKRedis *createRedis(
                const int8 *domainPort,//域名端口
                uint32 connectTimeout,//连接超时（毫秒）
                uint32 readWriteTimeout,//读写超时（毫秒）
                const int8 *password
        );
        bool checkRedis(AKRedis *redis);
        void destroyRedis(AKRedis *redis);

    private:
        AKRedisMap mRedisMap;
        AKFileMap mFileMap;
        AKHandleFile *mParent;
        AKHandleBuf *mHandleBuf;
        AKHandleDict *mHandleDict;
    };
}

#endif //AKSERVER_AKHANDLEFILE_H
