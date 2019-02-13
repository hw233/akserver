//
// Created by Sunny Yang on 2019-02-11.
//

#ifndef AKSERVER_AK_H
#define AKSERVER_AK_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../depend/MD5/md5.h"
#include "../depend/tinyxml/tinyxml.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <boost/timer.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <math.h>
#include <iostream>
#include <sstream>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#	include <Windows.h>
#else
#	include <unistd.h>
#	define ACL_UNIX
#endif
#include <mysql/mysql.h>
//#pragma comment(lib,"libmysql.lib")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "acl_cpp/lib_acl.hpp"
#include "acl_cpp/redis/redis_command.hpp"
//#include "lib_acl.h"
#ifdef _DEBUG
#	pragma comment(lib,"lib_acl_vc2017d.lib")
#	pragma comment(lib,"lib_acl_cpp_vc2017d.lib")
#else
//#	pragma comment(lib,"lib_acl_vc2017.lib")
//#	pragma comment(lib,"lib_acl_cpp_vc2017.lib")
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef char																		int8;
typedef short																		int16;
typedef int																			int32;
typedef long long																	int64;
typedef unsigned char																uint8;
typedef unsigned short																uint16;
typedef unsigned int																uint32;
typedef unsigned long long															uint64;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ak
{
    namespace net
    {
        class AKClient;
        class AKServer;
        class AKClientProxy;
    }
    namespace net2
    {
        class AKServer2;
        class AKClient2;
        class AKClientForScript;
        class AKServerForScript;
    }
    class AKRef;
    class AKSystem;
    class AKCore;
    class AKProcess;
    class AKProcessInfo;
    class AKScript;
    class AKOrder;
    class AKVar;
    class AKHandler;
    class AKRedis;
    namespace container
    {
        class AKBuf;
        class AKArr;
        class AKDict;
        class AKDictIterator;
    }
    namespace tinyxml2
    {
        typedef TiXmlDocument														XMLDocument;
        typedef TiXmlElement														XMLElement;
        typedef TiXmlComment														XMLComment;
        typedef TiXmlUnknown														XMLUnknown;
        typedef TiXmlAttribute														XMLAttribute;
        typedef TiXmlText															XMLText;
        typedef TiXmlDeclaration													XMLDeclaration;
        typedef TiXmlParsingData													XMLParsingData;
    }
    typedef boost::function<bool(AKScript *,AKOrder *)>								AKScriptFunc;
    typedef std::vector<std::string>												AKStringArray;
    typedef std::vector<AKVar *>													AKVarArray;
    typedef std::map<std::string,AKVar *>											AKVarMap;
    typedef std::map<AKVar *,AKVar *>												AKVarVarMap;
    typedef std::map<std::string,AKScriptFunc>										AKOrderMap;
    typedef std::map<std::string,std::string>										AKStringStringMap;
    typedef std::vector<std::string>												AKStringArray;
    typedef std::list<std::string>													AKStringList;
    typedef std::vector<AKOrder *>													AKOrderArray;
    typedef std::map<AKOrder *,AKOrder *>											AKOrderOrderMap;
    typedef std::list<int8 *>														AKBlockList;
    typedef std::list<int32>														Int32List;
    typedef std::vector<int8 *>														AKBlockArray;
    typedef std::vector<uint32>														AKUint32Array;
    typedef std::map<container::AKBuf *,container::AKBuf *>							AKBufMap;
    typedef std::map<container::AKArr *,container::AKArr *>							AKArrMap;
    typedef std::map<container::AKDict *,container::AKDict *>						AKDictMap;
    typedef std::map<container::AKDictIterator *,container::AKDictIterator *>		AKDictIteratorMap;
    typedef std::list<AKHandler *>													AKHandlerList;
    typedef std::map<std::string,AKHandler *>										AKHandlerMap;
    typedef std::map<std::string,tinyxml2::XMLDocument *>							AKStringXmlMap;
    typedef std::map<tinyxml2::XMLDocument *,tinyxml2::XMLDocument *>				AKXmlMap;
    typedef boost::shared_ptr<boost::thread>										AKThreadPtr;
    typedef std::list<AKThreadPtr>													AKThreadList;
    typedef std::map<uint32,AKVar *>												AKUint32VarMap;
    typedef std::map<AKProcess *,AKProcessInfo>										AKProcessInfoMap;
    typedef std::map<AKProcess *,AKProcess *>										AKProcessMap;
    typedef std::vector<AKCore *>													AKCoreArray;
    typedef std::map<AKScript *,AKScript *>											AKScriptMap;
    typedef std::map<std::string,AKScript *>										AKStringScriptMap;
    typedef std::map<net::AKClient *,net::AKClient *>								AKClientMap;
    typedef std::map<net::AKServer *,net::AKServer *>								AKServerMap;
    typedef std::map<net2::AKServerForScript *,net2::AKServerForScript *>			AKServerMap2;
    typedef std::map<net2::AKClientForScript *,net2::AKClientForScript *>			AKClientMap2;
    typedef std::map<net::AKClientProxy *,net::AKClientProxy *>						AKClientProxyMap;
    typedef std::list<net::AKClientProxy *>											AKClientProxyList;
    typedef std::map<MYSQL *,MYSQL *>												AKMysqlMap;
    typedef std::map<MYSQL_RES *,MYSQL_RES *>										AKRsMap;
    typedef std::map<FILE *,FILE *>													AKFileMap;
    typedef boost::shared_ptr<AKRedis>												AKRedisPtr;
    typedef std::map<AKRedis *,AKRedisPtr>											AKRedisMap;
    typedef std::map<AKRef *,AKRef *>												AKRefMap;

    namespace net
    {
        //数据描述
        struct AKDataDesc
        {
            void *data;//数据地址
            uint32 size;//数据尺寸
        };
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace ak;
#define AKDefineScriptFunc(name)\
	bool name(ak::AKScript *script,ak::AKOrder *order);
#define AKBindScriptFunc(script,name,func,remark,obj)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,obj,_1,_2)),\
	remark);
#define AKDefineSingleton(ClassName)\
	static ClassName &getSingleton()\
{\
	static ClassName singleton;\
	return singleton;\
}
#define AKDefineCreate(ClassName)\
	static ClassName *create()\
{\
	return new ClassName();\
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "AKRef.h"
#include "AKHandler.h"
#include "AKUseMutex.h"
#include "AKVar.h"
#include "AKOrder.h"
#include "AKCommon.h"
#include "container/AKStruct.h"
#include "container/AKStaticMem.h"
#include "container/AKBuf.h"
#include "container/AKArr.h"
#include "container/AKDict.h"
#include "net/AKClient.h"
#include "net/AKClientProxy.h"
#include "net/AKServer.h"
#include "net2/AKClient2.h"
#include "net2/AKClientForScript.h"
#include "net2/AKServer2.h"
#include "net2/AKServerForScript.h"
#include "AKScript.h"
#include "AKProcess.h"
#include "AKCore.h"
#include "AKSystem.h"
#include "AKDataTool.h"
#include "AKXmlMgr.h"
#include "AKRedis.h"

//...

#endif //AKSERVER_AK_H
