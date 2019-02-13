#include "ak.h"
#include "AKHandleBuf.h"
#include "AKHandleFile.h"
#include "AKHandleDict.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleFile::AKHandleFile()
            :mParent(NULL),
             mHandleBuf(NULL),
             mHandleDict(NULL)
    {
    }

    AKHandleFile::~AKHandleFile()
    {
        reset();
    }

    AKRedis *AKHandleFile::createRedis(
            const int8 *domainPort,//域名端口
            uint32 connectTimeout,//连接超时（毫秒）
            uint32 readWriteTimeout,//读写超时（毫秒）
            const int8 *password
    )
    {
        AKRedis *redis=new AKRedis(domainPort,connectTimeout,readWriteTimeout,password);

        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        handler->mRedisMap[redis]=AKRedisPtr(redis);
        return redis;
    }

    bool AKHandleFile::checkRedis(AKRedis *redis)
    {
        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        if(handler->mRedisMap.find(redis)!=handler->mRedisMap.end())return true;
        return false;
    }

    void AKHandleFile::destroyRedis(AKRedis *redis)
    {
        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        AKRedisMap::iterator it=handler->mRedisMap.find(redis);
        if(it==handler->mRedisMap.end())return;

        handler->mRedisMap.erase(it);
    }

    FILE *AKHandleFile::createFile(const int8 *filePath,const int8 *method)
    {
        FILE *fp=fopen(filePath,method);
        if(!fp)return NULL;

        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        handler->mFileMap[fp]=fp;
        return fp;
    }

    bool AKHandleFile::checkFile(FILE *fp)
    {
        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        if(handler->mFileMap.find(fp)!=handler->mFileMap.end())return true;
        return false;
    }

    void AKHandleFile::destroyFile(FILE *fp)
    {
        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        AKFileMap::iterator it=handler->mFileMap.find(fp);
        if(it==handler->mFileMap.end())return;

        fclose(it->second);
        handler->mFileMap.erase(it);
    }

    FILE *AKHandleFile::createPFile(const int8 *command,const int8 *method)
    {
#ifdef WIN32
        FILE *fp=_popen(command,method);
#else
        FILE *fp=popen(command,method);
#endif
        if(!fp)return NULL;

        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        handler->mFileMap[fp]=fp;
        return fp;
    }

    void AKHandleFile::destroyPFile(FILE *fp)
    {
        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        AKFileMap::iterator it=handler->mFileMap.find(fp);
        if(it==handler->mFileMap.end())return;

#ifdef WIN32
        _pclose(it->second);
#else
        pclose(it->second);
#endif
        handler->mFileMap.erase(it);
    }

    AKHandleBuf *AKHandleFile::getHandleBuf()
    {
        if(!mHandleBuf)
        {
            mHandleBuf=(AKHandleBuf *)getScript()->getHandler("AKHandleBuf");
        }
        return mHandleBuf;
    }

    AKHandleDict *AKHandleFile::getHandleDict()
    {
        if(!mHandleDict)
        {
            mHandleDict=(AKHandleDict *)getScript()->getHandler("AKHandleDict");
        }
        return mHandleDict;
    }

    void AKHandleFile::reset()
    {
        {
            AKFileMap::iterator it=mFileMap.begin();
            while(it!=mFileMap.end())
            {
                fclose(it->second);
                ++it;
            }
            mFileMap.clear();
        }
    }

    void AKHandleFile::init(AKScript *script)
    {
        AKHandler::init(script);

        //文件操作api
        AKQuickBindScriptFunc("fsize",&AKHandleFile::fsize,"bytes=fsize(filePath);//获取文件大小//返回-1表示文件不存在");
        AKQuickBindScriptFunc("fopen",&AKHandleFile::_fopen,"fp=fopen(filePath,method);//打开文件");
        AKQuickBindScriptFunc("popen",&AKHandleFile::mypopen,"fp=popen(command,method);//执行命令，并返回标准流文件");
        AKQuickBindScriptFunc("fcheck",&AKHandleFile::fcheck,"isOpen=fcheck(fp);//检查是否打开了文件操作");
        AKQuickBindScriptFunc("fseek",&AKHandleFile::_fseek,"fseek(fp,offset,method);//文件定位，method:0=开头;1=中间;2=尾部");
        AKQuickBindScriptFunc("ftell",&AKHandleFile::_ftell,"offset=ftell(fp);//获取文件偏移位置");
        AKQuickBindScriptFunc("fgetc",&AKHandleFile::_fgetc,"char=fgetc(fp);//获取文件当前字符，失败返回空字符串");
        AKQuickBindScriptFunc("fclose",&AKHandleFile::_fclose,"fclose(fp);//关闭文件");
        AKQuickBindScriptFunc("pclose",&AKHandleFile::mypclose,"pclose(fp);//关闭命令返回的标准流文件");
        AKQuickBindScriptFunc("getline",&AKHandleFile::_getline,"isSucc=getline(lineStr[,fp]);//从标准输入流stdin中获取一行字符串，如果取完则返回空字符串");
        AKQuickBindScriptFunc("fread",&AKHandleFile::_fread,"fread(fp,buf,bytes);//读文件");
        AKQuickBindScriptFunc("fwrite",&AKHandleFile::_fwrite,"fwrite(fp,buf);//写文件");
        AKQuickBindScriptFunc("log",&AKHandleFile::log,"log(logFilePath,logStr);//写日志文件");
        AKQuickBindScriptFunc("logFromBuf",&AKHandleFile::logFromBuf,"logFromBuf(logFilePath,buf);//写二进制日志文件");

        //文件系统api
        AKQuickBindScriptFunc("del",&AKHandleFile::del,"del(path);//删除目录或文件");
        AKQuickBindScriptFunc("createDir",&AKHandleFile::createDir,"createDir(dir);//创建多级目录");
        AKQuickBindScriptFunc("dirCreate",&AKHandleFile::createDir,"dirCreate(dir);//同createDir");
        AKQuickBindScriptFunc("dirExist",&AKHandleFile::dirExist,"dirExist(dirPath);//判断目录是否存在");
        AKQuickBindScriptFunc("fileList",&AKHandleFile::fileList,"fileDict=fileList(fileDict,findDir);//搜索findDir中的目录和文件，并保存到字典fileDict的key中，其中val:0=代表目录,1=文件");
        AKQuickBindScriptFunc("fileCopy",&AKHandleFile::fileCopy,"fileCopy(srcFilePath,destFilePath);//复制文件（覆盖）");
        AKQuickBindScriptFunc("fileExist",&AKHandleFile::fileExist,"fileExist(filePath);//判断文件是否存在");

        //redis api
        AKQuickBindScriptFunc("redis",&AKHandleFile::redis,"rd=redis(domain,port,cnnTimeout,readWriteTimeout[,password]);//创建redis操作对象");
        //AKQuickBindScriptFunc("redis_select",&AKHandleFile::redis_select,"rd=redis_select(rd,index);//选择库，index=0~15");
        //redis server
        AKQuickBindScriptFunc("redis_time",&AKHandleFile::redis_time,"redis_time(rd,t0,t1);//获取redis时间戳，t0=秒数,t1=这一秒内已过的微秒");
        AKQuickBindScriptFunc("redis_flushall",&AKHandleFile::redis_flushall,"redis_flushall(rd);//清空整个Redis服务器的数据(删除所有数据库的所有key)");
        AKQuickBindScriptFunc("redis_flushdb",&AKHandleFile::redis_flushdb,"redis_flushdb(rd);//清空当前数据库中的所有key");
        //redis key
        AKQuickBindScriptFunc("redis_del",&AKHandleFile::redis_del,"redis_del(rd,key1,...);redis_del(rd,arr(key1,...));//删除键");
        AKQuickBindScriptFunc("redis_exists",&AKHandleFile::redis_exists,"isExist=redis_exists(rd,key);//判断键是否存在");
        //redis string
        AKQuickBindScriptFunc("redis_set",&AKHandleFile::redis_set,"isSucc=redis_set(rd,key,val);//设置字符串键值");
        AKQuickBindScriptFunc("redis_get",&AKHandleFile::redis_get,"val=redis_get(rd,key);//获取字符串键值");
        AKQuickBindScriptFunc("redis_setnx",&AKHandleFile::redis_setnx,"isSucc=redis_setnx(rd,key,val);//设置键值，返回：1=成功;其它=失败(已存在或非字符串对象);");
        AKQuickBindScriptFunc("redis_getset",&AKHandleFile::redis_getset,"isSucc=redis_getset(rd,key,val);//获取旧值，并设置新值");
        AKQuickBindScriptFunc("redis_setex",&AKHandleFile::redis_setex,"isSucc=redis_setex(rd,key,dt,val);//设置键值并设置有效时间（秒），返回：1=成功;其它=失败;");
        AKQuickBindScriptFunc("redis_psetex",&AKHandleFile::redis_psetex,"isSucc=redis_psetex(rd,key,dt,val);//设置键值并设置有效时间（毫秒），返回：1=成功;其它=失败;");
        AKQuickBindScriptFunc("redis_setexnx",&AKHandleFile::redis_setexnx,"isSucc=redis_setexnx(rd,key,dt,val);//设置不存在的键值并设置有效时间（秒），返回：1=成功;其它=失败(已存在或非字符串对象);");
        AKQuickBindScriptFunc("redis_psetexnx",&AKHandleFile::redis_psetexnx,"isSucc=redis_psetexnx(rd,key,dt,val);//设置不存在的键值并设置有效时间（毫秒），返回：1=成功;其它=失败(已存在或非字符串对象);");
        //redis hash
        AKQuickBindScriptFunc("redis_hset",&AKHandleFile::redis_hset,"redis_hset(rd,key,attrib,val);//设置hash属性");
        AKQuickBindScriptFunc("redis_hget",&AKHandleFile::redis_hget,"val=redis_hget(rd,key,attrib);//获取hash属性");
        AKQuickBindScriptFunc("redis_hmset",&AKHandleFile::redis_hmset,"redis_hmset(rd,key,attribDict);//设置hash多组属性");
        AKQuickBindScriptFunc("redis_hmget",&AKHandleFile::redis_hmget,"attribValDict=redis_hmget(rd,key,attribNameArr);//获取hash多组属性");
        AKQuickBindScriptFunc("redis_hgetall",&AKHandleFile::redis_hgetall,"attribValDict=redis_hgetall(rd,key);//获取hash所有属性");
        AKQuickBindScriptFunc("redis_hdel",&AKHandleFile::redis_hdel,"redis_hdel(rd,key,attribName);//删除hash属性");
        AKQuickBindScriptFunc("redis_hexists",&AKHandleFile::redis_hexists,"isExist=redis_hexists(rd,key,attribName);//判断hash属性是否存在");
        //redis list
        AKQuickBindScriptFunc("redis_llen",&AKHandleFile::redis_llen,"count=redis_llen(rd,key);//获取list元素数量");
        AKQuickBindScriptFunc("redis_lpush",&AKHandleFile::redis_lpush,"count=redis_lpush(rd,key,val1[,val2,...]);//向list头部压入值，返回添加后list元素数量");
        AKQuickBindScriptFunc("redis_lpushx",&AKHandleFile::redis_lpushx,"count=redis_lpushx(rd,key,val);//如果key存在，则向list头部压入值，返回添加后list元素数量");
        AKQuickBindScriptFunc("redis_rpush",&AKHandleFile::redis_rpush,"count=redis_rpush(rd,key,val1[,val2,...]);//向list尾部压入值，返回添加后list元素数量");
        AKQuickBindScriptFunc("redis_rpushx",&AKHandleFile::redis_rpushx,"count=redis_rpushx(rd,key,val);//如果key存在，则向list尾部压入值，返回添加后list元素数量");
        AKQuickBindScriptFunc("redis_lpop",&AKHandleFile::redis_lpop,"val=redis_lpop(rd,key);//从list头部弹出值");
        AKQuickBindScriptFunc("redis_rpop",&AKHandleFile::redis_rpop,"val=redis_rpop(rd,key);//从list尾部弹出值");
        AKQuickBindScriptFunc("redis_lindex",&AKHandleFile::redis_lindex,"val=redis_lindex(rd,key,index);//从list中读取下标为index的值");
        AKQuickBindScriptFunc("redis_lrange",&AKHandleFile::redis_lrange,"valArr=redis_lrange(rd,key,startIndex,stopIndex);//从list中读取下标范围为[startIndex,stopIndex]的多个值");
        AKQuickBindScriptFunc("redis_lrem",&AKHandleFile::redis_lrem,"redis_lrem(rd,key,count,val);//移除列表中与参数val相等的元素，count==0:删除所有符合元素;count>0:从左向右删除数量为count;count<0:从右向左删除数量为count的绝对值");
        //redis set
        AKQuickBindScriptFunc("redis_sadd",&AKHandleFile::redis_sadd,"redis_sadd(rd,key,val1[,val2,...]);redis_sadd(rd,key,valArr);//将val加入集合key");
        AKQuickBindScriptFunc("redis_scard",&AKHandleFile::redis_scard,"redis_scard(rd,key);//获取集合元素数量");
        AKQuickBindScriptFunc("redis_smembers",&AKHandleFile::redis_smembers,"valArr=redis_smembers(rd,key);//获取集合所有元素");
        AKQuickBindScriptFunc("redis_sismember",&AKHandleFile::redis_sismember,"isMember=redis_sismember(rd,key,val);//判断是否存在于集合中");
        AKQuickBindScriptFunc("redis_spop",&AKHandleFile::redis_spop,"val=redis_spop(rd,key);//随机弹出并返回集合中一个元素");
        AKQuickBindScriptFunc("redis_srandmember",&AKHandleFile::redis_srandmember,"val=redis_srandmember(rd,key);//随机返回集合中一个元素");
        AKQuickBindScriptFunc("redis_srem",&AKHandleFile::redis_srem,"redis_srem(rd,key,val1,...);redis_srem(rd,key,arr(val1,...));//移除集合中元素");
        //redis sorted set
        AKQuickBindScriptFunc("redis_zadd",&AKHandleFile::redis_zadd,"redis_zadd(rd,key,score1,member1[,score2,member2...]);redis_zadd(rd,key,arr(score1,member1[,score2,member2...]));//向有序集合添加一个或多个成员，或者更新已存在成员的分数");
        AKQuickBindScriptFunc("redis_zrem",&AKHandleFile::redis_zrem,"redis_zrem(rd,key,member1[,member2...]);redis_zrem(rd,key,arr(member1[,member2...]));//有序集合删除一个或多个成员");
        AKQuickBindScriptFunc("redis_zrange",&AKHandleFile::redis_zrange,"redis_zrange(rd,key,start,stop,withscores,resultArr);//有序集合获取成员集合（升序），start=开始位置;stop=结束位置;withscores=是否同时返回score");
        AKQuickBindScriptFunc("redis_zrevrange",&AKHandleFile::redis_zrevrange,"redis_zrevrange(rd,key,start,stop,withscores,resultArr);//有序集合获取成员集合（升序），start=开始位置;stop=结束位置;withscores=是否同时返回score");
        AKQuickBindScriptFunc("redis_zrank",&AKHandleFile::redis_zrank,"redis_zrank(rd,key,member);//返回有序集key中成员member的排名（升序）");
        AKQuickBindScriptFunc("redis_zrevrank",&AKHandleFile::redis_zrevrank,"redis_zrevrank(rd,key,member);//返回有序集key中成员member的排名（降序）");
        AKQuickBindScriptFunc("redis_zcard",&AKHandleFile::redis_zcard,"redis_zcard(rd,key);//返回有序集数量");
    }

    //返回有序集数量
    bool AKHandleFile::redis_zcard(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        //键名
        const int8 *key=order->getParam(2)->toString();

        redis->zset.clear();
        int32 rt=redis->zset.zcard(key);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //返回有序集key中成员member的排名（降序）
    bool AKHandleFile::redis_zrevrank(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        //键名
        const int8 *key=order->getParam(2)->toString();

        //成员
        const int8 *member=order->getParam(3)->toString();

        redis->zset.clear();
        int32 rt=redis->zset.zrevrank(key,member);
        //if(rt==-1)
        //{
        //	onRunError(order,"操作超时");
        //	return false;//暂停执行脚本
        //}

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //返回有序集key中成员member的排名（升序）
    bool AKHandleFile::redis_zrank(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        //键名
        const int8 *key=order->getParam(2)->toString();

        //成员
        const int8 *member=order->getParam(3)->toString();

        redis->zset.clear();
        int32 rt=redis->zset.zrank(key,member);
        //if(rt==-1)
        //{
        //	onRunError(order,"操作超时");
        //	return false;//暂停执行脚本
        //}

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //有序集合获取成员集合（降序），start=开始位置;stop=结束位置;withscores=是否同时返回score
    bool AKHandleFile::redis_zrevrange(AKScript *script,AKOrder *order)
    {
        if(!checkParams(6,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        //键名
        const int8 *key=order->getParam(2)->toString();

        //开始位置
        int32 begin=order->getParam(3)->toInt();

        //结束位置
        int32 stop=order->getParam(4)->toInt();

        //是否同时返回score
        int32 withscores=order->getParam(5)->toInt();

        //结果数组
        container::AKArr *resultArr=(container::AKArr *)order->getParam(6)->toHandle();
        if(!script->getCore()->checkArr(resultArr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }
        resultArr->clear();

        int rt=0;
        if(withscores==1)
        {
            std::vector<std::pair<acl::string,double> > result2;
            redis->zset.clear();
            rt=redis->zset.zrevrange_with_scores(key,begin,stop,result2);
            if(rt==-1)
            {
                onRunError(order,"操作超时");
                return false;//暂停执行脚本
            }
            uint32 c=result2.size();
            for(uint32 i=0;i<c;++i)
            {
                std::pair<acl::string,double> &info=result2[i];
                resultArr->push(info.first.c_str());
                resultArr->push(info.second);
            }
        }
        else
        {
            std::vector<acl::string> result;
            redis->zset.clear();
            rt=redis->zset.zrevrange(key,begin,stop,&result);
            if(rt==-1)
            {
                onRunError(order,"操作超时");
                return false;//暂停执行脚本
            }
            uint32 c=result.size();
            for(uint32 i=0;i<c;++i)
            {
                resultArr->push(result[i].c_str());
            }
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //有序集合获取成员集合（升序），start=开始位置;stop=结束位置;withscores=是否同时返回score
    bool AKHandleFile::redis_zrange(AKScript *script,AKOrder *order)
    {
        if(!checkParams(6,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        //键名
        const int8 *key=order->getParam(2)->toString();

        //开始位置
        int32 begin=order->getParam(3)->toInt();

        //结束位置
        int32 stop=order->getParam(4)->toInt();

        //是否同时返回score
        int32 withscores=order->getParam(5)->toInt();

        //结果数组
        container::AKArr *resultArr=(container::AKArr *)order->getParam(6)->toHandle();
        if(!script->getCore()->checkArr(resultArr))
        {
            onRunError(order,"无效数组");
            return false;//暂停执行脚本
        }
        resultArr->clear();

        int rt=0;
        if(withscores==1)
        {
            std::vector<std::pair<acl::string,double> > result2;
            redis->zset.clear();
            rt=redis->zset.zrange_with_scores(key,begin,stop,result2);
            if(rt==-1)
            {
                onRunError(order,"操作超时");
                return false;//暂停执行脚本
            }
            uint32 c=result2.size();
            for(uint32 i=0;i<c;++i)
            {
                std::pair<acl::string,double> &info=result2[i];
                resultArr->push(info.first.c_str());
                resultArr->push(info.second);
            }
        }
        else
        {
            std::vector<acl::string> result;
            redis->zset.clear();
            rt=redis->zset.zrange(key,begin,stop,&result);
            if(rt==-1)
            {
                onRunError(order,"操作超时");
                return false;//暂停执行脚本
            }
            uint32 c=result.size();
            for(uint32 i=0;i<c;++i)
            {
                resultArr->push(result[i].c_str());
            }
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向有序集合添加一个或多个成员，或者更新已存在成员的分数
    bool AKHandleFile::redis_zrem(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        std::vector<acl::string> members;

        if(order->getParam(3)->getType()==AKVar::typeHandle)//传入元素数组
        {
            container::AKArr *arr=(container::AKArr *)order->getParam(3)->toHandle();
            if(!script->getCore()->checkArr(arr))
            {
                onRunError(order,"无效数组");
                return false;//暂停执行脚本
            }

            uint32 c=arr->size();
            for(uint32 i=0;i<c;++i)
            {
                const int8 *member=arr->get(i)->toString();//成员名
                members.push_back(member);
            }
        }
        else//传入多个元素
        {
            for(uint32 i=3;order->getParam(i);++i)
            {
                const int8 *member=order->getParam(i)->toString();//值
                members.push_back(member);
            }
        }

        redis->zset.clear();
        int rt=redis->zset.zrem(key,members);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向有序集合添加一个或多个成员，或者更新已存在成员的分数
    bool AKHandleFile::redis_zadd(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        //std::vector<acl::string> members;
        std::map<acl::string, double> members;

        if(order->getParam(3)->getType()==AKVar::typeHandle)//传入元素数组
        {
            container::AKArr *arr=(container::AKArr *)order->getParam(3)->toHandle();
            if(!script->getCore()->checkArr(arr))
            {
                onRunError(order,"无效数组");
                return false;//暂停执行脚本
            }

            uint32 c=arr->size();
            for(uint32 i=0;i<c;i+=2)
            {
                double score=arr->get(i)->toNumber();//分数
                const int8 *member=arr->get(i+1)->toString();//成员名
                members[member]=score;
            }
        }
        else//传入多个元素
        {
            for(uint32 i=3;order->getParam(i);i+=2)
            {
                double score=order->getParam(i)->toNumber();//分数
                const int8 *member=order->getParam(i+1)->toString();//值
                members[member]=score;
            }
        }

        redis->zset.clear();
        int rt=redis->zset.zadd(key,members);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //移除列表中与参数val相等的元素，count==0:删除所有符合元素;count>0:从左向右删除数量为count;count<0:从右向左删除数量为count的绝对值
    bool AKHandleFile::redis_lrem(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        int32 count=order->getParam(3)->toInt();
        const int8 *val=order->getParam(4)->toString();//值

        redis->list.clear();
        int rt=redis->list.lrem(key,count,val);//返回删除数量
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //移除集合中元素
    bool AKHandleFile::redis_srem(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        std::vector<acl::string> members;

        if(order->getParam(3)->getType()==AKVar::typeHandle)//传入元素数组
        {
            container::AKArr *arr=(container::AKArr *)order->getParam(3)->toHandle();
            if(!script->getCore()->checkArr(arr))
            {
                onRunError(order,"无效数组");
                return false;//暂停执行脚本
            }

            uint32 c=arr->size();
            for(uint32 i=0;i<c;++i)
            {
                const int8 *val=arr->get(i)->toString();//值
                members.push_back(val);
            }
        }
        else//传入多个元素
        {
            for(uint32 i=3;order->getParam(i);++i)
            {
                const int8 *val=order->getParam(i)->toString();//值
                members.push_back(val);
            }
        }

        redis->set.clear();
        int rt=redis->set.srem(key,members);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //随机返回集合中一个元素
    bool AKHandleFile::redis_srandmember(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        redis->set.clear();
        acl::string val;
        if(redis->set.srandmember(key,val)<=0)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //随机弹出并返回集合中一个元素
    bool AKHandleFile::redis_spop(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        redis->set.clear();
        acl::string val;
        if(!redis->set.spop(key,val))
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断是否存在于集合中
    bool AKHandleFile::redis_sismember(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *val=order->getParam(3)->toString();//值

        redis->set.clear();
        int rt=0;
        if(redis->set.sismember(key,val))rt=1;

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取集合所有元素
    bool AKHandleFile::redis_smembers(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        std::vector<acl::string> members;
        container::AKArr *arr=script->getCore()->createArr();

        redis->set.clear();
        if(redis->set.smembers(key,&members)==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        uint32 c=members.size();
        for(uint32 i=0;i<c;++i)
        {
            arr->push(members[i].c_str());
        }

        out->setValue((void *)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取集合元素数量
    bool AKHandleFile::redis_scard(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        redis->set.clear();
        int rt=redis->set.scard(key);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //将val加入集合key
    bool AKHandleFile::redis_sadd(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        std::vector<acl::string> members;

        if(order->getParam(3)->getType()==AKVar::typeHandle)//传入元素数组
        {
            container::AKArr *arr=(container::AKArr *)order->getParam(3)->toHandle();
            if(!script->getCore()->checkArr(arr))
            {
                onRunError(order,"无效数组");
                return false;//暂停执行脚本
            }

            uint32 c=arr->size();
            for(uint32 i=0;i<c;++i)
            {
                const int8 *val=arr->get(i)->toString();//值
                members.push_back(val);
            }
        }
        else//传入多个元素
        {
            for(uint32 i=3;order->getParam(i);++i)
            {
                const int8 *val=order->getParam(i)->toString();//值
                members.push_back(val);
            }
        }

        redis->set.clear();
        int rt=redis->set.sadd(key,members);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置不存在的键值并设置有效时间（毫秒），返回：1=成功;其它=失败(已存在或非字符串对象);
    bool AKHandleFile::redis_psetexnx(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        uint32 dt=order->getParam(3)->toUint();//有效时间（秒）
        const int8 *val=order->getParam(4)->toString();//值

        redis->string.clear();
        bool rt=redis->akcommand.psetexnx(key,val,dt);
        if(rt)out->setValue((double)1);//成功
        else out->setValue((double)0);//失败

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置不存在的键值并设置有效时间（秒），返回：1=成功;其它=失败(已存在或非字符串对象);
    bool AKHandleFile::redis_setexnx(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        uint32 dt=order->getParam(3)->toUint();//有效时间（秒）
        const int8 *val=order->getParam(4)->toString();//值

        redis->string.clear();
        bool rt=redis->akcommand.setexnx(key,val,dt);
        if(rt)out->setValue((double)1);//成功
        else out->setValue((double)0);//失败

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置键值并设置有效时间（毫秒），返回：1=成功;其它=失败(已存在或非字符串对象);
    bool AKHandleFile::redis_psetex(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        uint32 dt=order->getParam(3)->toUint();//有效时间（秒）
        const int8 *val=order->getParam(4)->toString();//值

        redis->string.clear();
        bool rt=redis->string.psetex(key,val,dt);
        if(rt)out->setValue((double)1);//成功
        else out->setValue((double)0);//失败

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置键值并设置有效时间（秒），返回：1=成功;其它=失败(已存在或非字符串对象);
    bool AKHandleFile::redis_setex(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        uint32 dt=order->getParam(3)->toUint();//有效时间（秒）
        const int8 *val=order->getParam(4)->toString();//值

        redis->string.clear();
        bool rt=redis->string.setex(key,val,dt);
        if(rt)out->setValue((double)1);//成功
        else out->setValue((double)0);//失败

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从list中读取下标范围为[startIndex,stopIndex]的多个值
    bool AKHandleFile::redis_lrange(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        int32 startIndex=order->getParam(3)->toInt();//开始下标
        int32 stopIndex=order->getParam(4)->toInt();//结束下标

        std::vector<acl::string> result;
        redis->list.clear();
        if(!redis->list.lrange(key,startIndex,stopIndex,&result))
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        container::AKArr *arr=script->getCore()->createArr();
        uint32 c=result.size();
        for(uint32 i=0;i<c;++i)
        {
            arr->set(i,result[i].c_str());
        }

        out->setValue((void *)arr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从list中读取下标为index的值
    bool AKHandleFile::redis_lindex(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        uint32 index=order->getParam(3)->toUint();//下标

        acl::string val;
        redis->list.clear();
        if(!redis->list.lindex(key,index,val))
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从list尾部弹出值
    bool AKHandleFile::redis_rpop(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        acl::string val;
        redis->list.clear();
        int count=redis->list.rpop(key,val);//返回弹出数量
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取list元素数量
    bool AKHandleFile::redis_llen(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        redis->list.clear();
        int count=redis->list.llen(key);//返回弹出数量
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)count);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从list头部弹出值
    bool AKHandleFile::redis_lpop(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        acl::string val;
        redis->list.clear();
        int count=redis->list.lpop(key,val);//返回弹出数量
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //如果key存在，则向list尾部压入值，返回添加后list元素数量
    bool AKHandleFile::redis_rpushx(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *val=order->getParam(3)->toString();//值

        redis->list.clear();
        int count=redis->list.rpushx(key,val);//返回添加完后当前列表对象中的元素个数
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)count);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向list头尾压入值，返回添加后list元素数量
    bool AKHandleFile::redis_rpush(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        std::vector<const int8 *> values;
        for(int32 i=3;;++i)
        {
            if(!order->getParam(i))break;
            const int8 *val=order->getParam(i)->toString();//值
            values.push_back(val);
        }

        redis->list.clear();
        int count=redis->list.rpush(key,values);//返回添加完后当前列表对象中的元素个数
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)count);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //如果key存在，则向list头部压入值，返回添加后list元素数量
    bool AKHandleFile::redis_lpushx(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *val=order->getParam(3)->toString();//值

        redis->list.clear();
        int count=redis->list.lpushx(key,val);//返回添加完后当前列表对象中的元素个数
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)count);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //向list头部压入值，返回添加后list元素数量
    bool AKHandleFile::redis_lpush(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        std::vector<const int8 *> values;
        for(int32 i=3;;++i)
        {
            if(!order->getParam(i))break;
            const int8 *val=order->getParam(i)->toString();//值
            values.push_back(val);
        }

        redis->list.clear();
        int count=redis->list.lpush(key,values);//返回添加完后当前列表对象中的元素个数
        if(count==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)count);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //清空当前数据库中的所有key
    bool AKHandleFile::redis_flushdb(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        redis->server.clear();
        if(!redis->server.flushdb())
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //清空整个Redis服务器的数据(删除所有数据库的所有key)
    bool AKHandleFile::redis_flushall(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }

        redis->server.clear();
        if(!redis->server.flushall())
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    ////选择库，index=0~15
    //bool AKHandleFile::redis_select(AKScript *script,AKOrder *order)
    //{
    //	if(!checkParams(2,script,order))return false;
    //	AKVar *out=order->getParam(0);//接收返回的变量
    //	out->setType(AKVar::typeNumber);//设置返回类型

    //	AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
    //	if(!checkRedis(redis))
    //	{
    //		onRunError(order,"无效redis句柄");
    //		return false;//暂停执行脚本
    //	}
    //
    //	//库索引
    //	uint32 index=order->getParam(2)->toUint();


    //	time_t time;//秒数
    //	int microsec=0;//这一秒内已过微秒数
    //	redis->server.clear();
    //	if(!redis->server.get_time(time,microsec))
    //	{
    //		onRunError(order,"操作超时");
    //		return false;//暂停执行脚本
    //	}

    //	t0->setValue((double)time);
    //	t1->setValue((double)microsec);

    //	out->setValue((double)1);
    //	script->nextOrder();//下一行指令
    //	return true;//继续执行脚本
    //}

    //获取redis时间戳，t0=秒数,t1=这一秒内已过的微秒
    bool AKHandleFile::redis_time(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        AKVar *t0=order->getParam(2);//秒数
        AKVar *t1=order->getParam(3);//这一秒内已过微秒数


        time_t time;//秒数
        int microsec=0;//这一秒内已过微秒数
        redis->server.clear();
        if(!redis->server.get_time(time,microsec))
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        t0->setValue((double)time);
        t1->setValue((double)microsec);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取旧值，并设置新值
    bool AKHandleFile::redis_getset(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *val=order->getParam(3)->toString();//值

        acl::string oldVal;//旧值
        redis->string.clear();
        if(!redis->string.getset(key,val,oldVal))
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(oldVal);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取字符串键值
    bool AKHandleFile::redis_get(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        acl::string val;
        redis->string.clear();
        if(!redis->string.get(key,val))
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置字符串键值
    bool AKHandleFile::redis_set(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *val=order->getParam(3)->toString();//值

        redis->string.clear();
        bool rt=redis->string.set(key,val);
        if(rt)out->setValue((double)1);//成功
        else out->setValue((double)0);//失败

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置键值，返回：1=成功;其它=失败(已存在或非字符串对象);
    bool AKHandleFile::redis_setnx(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *val=order->getParam(3)->toString();//值

        redis->string.clear();
        int rt=redis->string.setnx(key,val);
        if(rt==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断键是否存在
    bool AKHandleFile::redis_exists(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        redis->key.clear();
        if(redis->key.exists(key))out->setValue((double)1);
        else out->setValue((double)0);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除键
    bool AKHandleFile::redis_del(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        //const int8 *key=order->getParam(2)->toString();//键名
        std::vector<acl::string> keys;

        if(order->getParam(2)->getType()==AKVar::typeHandle)//传入元素数组
        {
            container::AKArr *arr=(container::AKArr *)order->getParam(3)->toHandle();
            if(!script->getCore()->checkArr(arr))
            {
                onRunError(order,"无效数组");
                return false;//暂停执行脚本
            }

            uint32 c=arr->size();
            for(uint32 i=0;i<c;++i)
            {
                const int8 *val=arr->get(i)->toString();//值
                keys.push_back(val);
            }
        }
        else//传入多个元素
        {
            for(uint32 i=2;order->getParam(i);++i)
            {
                const int8 *val=order->getParam(i)->toString();//值
                keys.push_back(val);
            }
        }

        redis->key.clear();
        if(redis->key.del(keys)==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建redis操作对象
    bool AKHandleFile::redis(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *domain=order->getParam(1)->toString();//域名
        const int8 *port=order->getParam(2)->toString();//端口
        uint32 cnnTimeout=order->getParam(3)->toUint();//连接超时（毫秒）
        uint32 readWriteTimeout=order->getParam(4)->toUint();//读写超时（毫秒）
        const int8 *password="";
        if(order->getParam(5))
        {
            password=order->getParam(5)->toString();
        }

        std::string domainPort=domain;
        domainPort+=":";
        domainPort+=port;
        AKRedis *redis=new AKRedis(domainPort.c_str(),cnnTimeout,readWriteTimeout,password);
        AKHandleFile *handler=(AKHandleFile *)getTopParent();
        handler->mRedisMap[redis]=AKRedisPtr(redis);

        out->setValue((void *)redis);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断hash属性是否存在
    bool AKHandleFile::redis_hexists(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *attrib=order->getParam(3)->toString();//属性名

        redis->hash.clear();
        if(redis->hash.hexists(key,attrib))out->setValue((double)1);
        else out->setValue((double)0);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除hash属性
    bool AKHandleFile::redis_hdel(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *attrib=order->getParam(3)->toString();//属性名

        redis->hash.clear();
        if(redis->hash.hdel(key,attrib)==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取hash全部属性
    bool AKHandleFile::redis_hgetall(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        redis->hash.clear();
        std::map<acl::string, acl::string> rt;
        if(redis->hash.hgetall(key,rt)==false)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        container::AKDict *newDict=script->getCore()->createDict();
        std::map<acl::string, acl::string>::const_iterator cit;
        for (cit =rt.begin();cit!= rt.end();++cit)
        {
            newDict->set(cit->first.c_str(),cit->second.c_str());
        }

        out->setValue((void *)newDict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取hash多组属性
    bool AKHandleFile::redis_hmget(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        //属性名数组
        container::AKArr *attribNameArr=(container::AKArr *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkArr(attribNameArr))
        {
            onRunError(order,"无效属性名数组");
            return false;//暂停执行脚本
        }
        uint32 attribNameCount=attribNameArr->size();//属性名数量
        if(attribNameCount==0)
        {
            onRunError(order,"属性名数组不能为空");
            return false;//暂停执行脚本
        }
        container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
        const int8 **attribNameArr2=(const int8 **)mem.alloc(sizeof(int8 *)*attribNameCount);
        for(uint32 i=0;i<attribNameCount;++i)
        {
            attribNameArr2[i]=attribNameArr->get(i)->toString();
        }

        redis->hash.clear();
        std::vector<acl::string> rt;
        if(redis->hash.hmget(key,attribNameArr2,attribNameCount,&rt)==false)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        container::AKDict *newDict=script->getCore()->createDict();
        for(uint32 i=0;i<attribNameCount;++i)
        {
            newDict->set(attribNameArr2[i],rt[i].c_str());
        }

        out->setValue((void *)newDict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置hash多组属性
    bool AKHandleFile::redis_hmset(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名

        //属性-值字典
        container::AKDict *dict=(container::AKDict *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }
        AKUint32VarMap &uint32VarMap=dict->mUint32VarMap;
        AKVarMap &varMap=dict->mVarMap;

        int8 tmp[20];
        std::map<acl::string, acl::string> attrs;
        container::AKDictIterator it;
        it=dict->begin();
        while(it)
        {
            const int8 *key=it.keyString();
            if(key)//字符串key
            {
                attrs[key]=it.val()->toString();
            }
            else//数值key
            {
                sprintf(tmp,"%lu",it.keyUint32());
                attrs[tmp]=it.val()->toString();
            }
            ++it;
        }

        redis->hash.clear();
        if(redis->hash.hmset(key,attrs)==false)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //设置hash属性
    bool AKHandleFile::redis_hset(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *attrib=order->getParam(3)->toString();//属性名
        const int8 *val=order->getParam(4)->toString();//值

        redis->hash.clear();
        if(redis->hash.hset(key,attrib,val)==-1)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取hash属性
    bool AKHandleFile::redis_hget(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        AKRedis *redis=(AKRedis *)order->getParam(1)->toHandle();
        if(!checkRedis(redis))
        {
            onRunError(order,"无效redis句柄");
            return false;//暂停执行脚本
        }
        const int8 *key=order->getParam(2)->toString();//键名
        const int8 *attrib=order->getParam(3)->toString();//属性名

        redis->hash.clear();
        acl::string rt;
        if(redis->hash.hget(key,attrib,rt)==false)
        {
            onRunError(order,"操作超时");
            return false;//暂停执行脚本
        }

        out->setValue(rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //删除目录或文件
    bool AKHandleFile::del(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *dirPath=order->getParam(1)->toString();//目录
        boost::filesystem::remove_all(dirPath);

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断目录是否存在
    bool AKHandleFile::dirExist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *dirPath=order->getParam(1)->toString();//目录
        if(boost::filesystem::exists(dirPath))//目录存在
        {
            out->setValue((double)1);
        }
        else//目录不存在
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断文件是否存在
    bool AKHandleFile::fileExist(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *filePath=order->getParam(1)->toString();//文件路径
        FILE *fp=fopen(filePath,"rb");
        if(fp)//文件存在
        {
            fclose(fp);
            out->setValue((double)1);
        }
        else//文件不存在
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //复制文件（覆盖）
    bool AKHandleFile::fileCopy(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *srcFilePath=order->getParam(1)->toString();//源文件
        const int8 *destFilePath=order->getParam(2)->toString();//目标文件
        boost::filesystem::copy_file(
                boost::filesystem::path(srcFilePath),
                boost::filesystem::path(destFilePath),
                boost::filesystem::copy_option::overwrite_if_exists
        );

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //搜索findDir中的目录和文件，并保存到字典fileDict中
    bool AKHandleFile::fileList(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        container::AKDict *dict=(container::AKDict *)order->getParam(1)->toHandle();
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }
        dict->clear();

        const int8 *findDir=order->getParam(2)->toString();//查找的目录
        boost::filesystem::path findDirPath(findDir);
        AKCommon::fileList(*dict,findDirPath);

        out->setValue((void *)dict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建多级目录
    bool AKHandleFile::createDir(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *dir=order->getParam(1)->toString();
        bool rt=AKCommon::createDir(dir);

        if(rt)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取文件大小
    bool AKHandleFile::fsize(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        double bytes=-1;
        const int8 *filePath=order->getParam(1)->toString();//文件路径
        FILE *fp=fopen(filePath,"rb");
        if(fp)
        {
            fseek(fp,0,SEEK_END);
            bytes=ftell(fp);
            fclose(fp);
        }

        out->setValue((double)bytes);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //写文件
    bool AKHandleFile::_fwrite(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }

        //接收返回数据的buf
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        //写入
        uint32 c=buf->getBlockCount();
        for(uint32 i=0;i<c;++i)
        {
            uint32 blockSize=0;
            int8 *data=buf->getBlockData(i,blockSize);
            fwrite(data,blockSize,1,fp);
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //读文件
    bool AKHandleFile::_fread(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }

        //接收返回数据的buf
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        //读取字节数
        uint32 bytes=order->getParam(3)->toUint();

        //读取
        container::AKUseStaticMem mem(script->getCore()->mStaticMem);//临时缓存
        int8 *tmp=mem.alloc(bytes);
        double rt=fread(tmp,bytes,1,fp);
        buf->clear();
        if(rt)
        {
            buf->write(tmp,bytes);
            buf->setPos(0);
        }

        out->setValue((double)rt);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //文件定位，method:0=开头;1=中间;2=尾部
    bool AKHandleFile::_fseek(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }
        int32 offset=order->getParam(2)->toInt();
        int32 method=order->getParam(3)->toInt();

        fseek(fp,offset,method);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取文件偏移位置
    bool AKHandleFile::_ftell(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }

        double offset=ftell(fp);

        out->setValue((double)offset);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //关闭文件
    bool AKHandleFile::_fgetc(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }

        int8 ch=fgetc(fp);
        if(ch==-1)
        {
            out->setValue((const int8 *)"");
        }
        else
        {
            int8 tmp[2];
            tmp[0]=ch;
            tmp[1]='\0';
            out->setValue((const int8 *)tmp);
        }
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //关闭文件
    bool AKHandleFile::_fclose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }

        destroyFile(fp);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //关闭命令返回的标准流文件
    bool AKHandleFile::mypclose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(!checkFile(fp))
        {
            onRunError(order,"无效文件句柄");
            return false;//暂停执行脚本
        }

        destroyPFile(fp);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //检查是否打开了文件操作
    bool AKHandleFile::fcheck(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=(FILE *)order->getParam(1)->toHandle();
        if(checkFile(fp))
        {
            out->setValue((double)1);
        }
        else
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //打开文件
    bool AKHandleFile::_fopen(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *filePath=order->getParam(1)->toString();//文件路径
        const int8 *method=order->getParam(2)->toString();//打开文件方式

        FILE *fp=createFile(filePath,method);

        out->setValue((void *)fp);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //执行命令，并返回标准流文件
    bool AKHandleFile::mypopen(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *command=order->getParam(1)->toString();//命令
        const int8 *method=order->getParam(2)->toString();//打开文件方式

        FILE *fp=createPFile(command,method);

        out->setValue((void *)fp);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //从标准输入流中获取一行字符串，如果取完则返回空字符串
    bool AKHandleFile::_getline(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        FILE *fp=stdin;
        if(order->getParam(2))
        {
            fp=(FILE *)order->getParam(2)->toHandle();
            if(!checkFile(fp))
            {
                onRunError(order,"无效文件句柄");
                return false;//暂停执行脚本
            }
        }

        bool isSucc=true;
        std::string lineStr="";
        char chs[2];chs[1]='\0';
        char ch;
        uint32 i=0;
        while(true)
        {
            ch=fgetc(fp);
            if(ch==EOF)
            {
                if(i==0)isSucc=false;
                break;
            }
            if(ch=='\n'||ch=='\r')break;
            chs[0]=ch;
            lineStr+=chs;
            ++i;
        }

        order->getParam(1)->setValue((const int8 *)lineStr.c_str());
        if(isSucc)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //写日志文件
    bool AKHandleFile::log(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *filePath=order->getParam(1)->toString();//日志文件路径
        std::string &logStr=order->getParam(2)->toStdString();//日志信息

        AKCommon::log(filePath,logStr);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //写二进制日志文件
    bool AKHandleFile::logFromBuf(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        const int8 *filePath=order->getParam(1)->toString();//日志文件路径
        container::AKBuf *buf=(container::AKBuf *)order->getParam(2)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        FILE *fp=fopen(filePath,"ab");
        if(fp)
        {
            //写入
            uint32 c=buf->getBlockCount();
            for(uint32 i=0;i<c;++i)
            {
                uint32 blockSize=0;
                int8 *data=buf->getBlockData(i,blockSize);
                fwrite(data,blockSize,1,fp);
            }
            fclose(fp);
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

}
