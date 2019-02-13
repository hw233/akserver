#include "ak.h"
#include "AKScript.h"
#include "AKHandleBuf.h"
#include "AKHandleMysql.h"
#include "AKGlobal.h"

#define AKQuickBindScriptFunc(name,func,remark)\
	script->regOrderFunction(name,\
	ak::AKScriptFunc(boost::bind(func,this,_1,_2)),\
	remark);

namespace ak
{
    AKHandleMysql::AKHandleMysql()
            :mParent(NULL),
             mHandleBuf(NULL)
    {
    }

    AKHandleMysql::~AKHandleMysql()
    {
        reset();
    }

    MYSQL_RES *AKHandleMysql::createRs(MYSQL *mysql)
    {
        MYSQL_RES *rs=mysql_store_result(mysql);//创建记录集
        mRsMap[rs]=rs;
        return rs;
    }

    bool AKHandleMysql::checkRs(MYSQL_RES *rs)
    {
        if(mRsMap.find(rs)!=mRsMap.end())return true;
        if(!mParent)
        {
            AKScript *parentScript=getScript()->getParentScript();
            if(!parentScript)
            {
                return false;
            }
            mParent=(AKHandleMysql *)parentScript->getHandler("AKHandleMysql");
        }
        return mParent->checkRs(rs);
    }

    void AKHandleMysql::destroyRs(MYSQL_RES *rs)
    {
        AKRsMap::iterator it=mRsMap.find(rs);
        if(it==mRsMap.end())return;

        mysql_free_result(it->second);
        mRsMap.erase(it);
    }

    MYSQL *AKHandleMysql::createMysql()
    {
        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        MYSQL *mysql=mysql_init(NULL);
        handler->mMysqlMap[mysql]=mysql;
        return mysql;
    }

    bool AKHandleMysql::checkMysql(MYSQL *mysql)
    {
        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        if(handler->mMysqlMap.find(mysql)!=handler->mMysqlMap.end())return true;
        return false;
    }

    void AKHandleMysql::destroyMysql(MYSQL *mysql)
    {
        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKMysqlMap::iterator it=handler->mMysqlMap.find(mysql);
        if(it==handler->mMysqlMap.end())return;

        mysql_close(it->second);
        handler->mMysqlMap.erase(it);
    }

    AKHandleBuf *AKHandleMysql::getHandleBuf()
    {
        if(!mHandleBuf)
        {
            mHandleBuf=(AKHandleBuf *)getScript()->getHandler("AKHandleBuf");
        }
        return mHandleBuf;
    }

    void AKHandleMysql::reset()
    {
        {
            AKMysqlMap::iterator it=mMysqlMap.begin();
            while(it!=mMysqlMap.end())
            {
                mysql_close(it->second);
                ++it;
            }
            mMysqlMap.clear();
        }

        {
            AKRsMap::iterator it=mRsMap.begin();
            while(it!=mRsMap.end())
            {
                mysql_free_result(it->second);
                ++it;
            }
            mRsMap.clear();
        }
    }

    //格式化字符串（转化mysql语句中会引起转义的部分）
    void formatStr(std::string &destStr)
    {
        AKCommon::strReplace(destStr,"\\","\\\\");
        AKCommon::strReplace(destStr,"'","\\'");
        AKCommon::strReplace(destStr,"`","\\`");
    }

    void AKHandleMysql::init(AKScript *script)
    {
        AKHandler::init(script);

        //基本mysql API
        AKQuickBindScriptFunc("mysqlCnn",&AKHandleMysql::mysqlCnn,"cnn=mysqlCnn(ip,port,usr,pwd[,db]);//连接mysql");
        AKQuickBindScriptFunc("mysqlPing",&AKHandleMysql::mysqlPing,"mysqlPing(cnn);//判断cnn是否断线，如果断线则会自动重连");
        AKQuickBindScriptFunc("mysqlClose",&AKHandleMysql::mysqlClose,"mysqlClose(cnn);//关闭mysql");
        AKQuickBindScriptFunc("mysqlExec",&AKHandleMysql::mysqlExec,"mysqlExec(cnn,sql);//执行mysql");
        AKQuickBindScriptFunc("mysqlCreateRs",&AKHandleMysql::mysqlCreateRs,"rs=mysqlCreateRs(cnn,sql);//创建mysql记录集");
        AKQuickBindScriptFunc("mysqlDestroyRs",&AKHandleMysql::mysqlDestroyRs,"mysqlDestroyRs(rs);//销毁mysql记录集");
        AKQuickBindScriptFunc("mysqlGetCols",&AKHandleMysql::mysqlGetCols,"cols=mysqlGetCols(rs);//获取记录集列数");
        AKQuickBindScriptFunc("mysqlGetColNames",&AKHandleMysql::mysqlGetColNames,"colNameArr=mysqlGetColNames(rs);//获取记录集列名数组");
        AKQuickBindScriptFunc("mysqlNextRow",&AKHandleMysql::mysqlNextRow,"isExistRow=mysqlNextRow(rs);//获取记录集下一行，并返回行是否存在");
        AKQuickBindScriptFunc("mysqlIsBinCol",&AKHandleMysql::mysqlIsBinCol,"isBinCol=mysqlIsBinCol(rs,col);//判断列类型是否为二进制数据");
        AKQuickBindScriptFunc("mysqlGetColVal",&AKHandleMysql::mysqlGetColVal,"val=mysqlGetColVal(rs,col);//获取记录集列的字符串数据");
        AKQuickBindScriptFunc("mysqlGetColBin",&AKHandleMysql::mysqlGetColBin,"mysqlGetColBin(rs,col,dataBuf);//获取记录集列的二进制数据");
        AKQuickBindScriptFunc("mysqlInsertID",&AKHandleMysql::mysqlInsertID,"mysqlInsertID(cnn);//获取最后插入的记录ID");
        AKQuickBindScriptFunc("mysqlGetTables",&AKHandleMysql::mysqlGetTables,"tableNameArr=mysqlGetTables(cnn,dbName,tableNameMatch);//根据tableNameMatch条件，获取表名数组");

        //表维护API
        AKQuickBindScriptFunc("mysqlTable",&AKHandleMysql::mysqlTable,"mysqlTable(dbName,tableName[,engineName]);//开始定义表,engineName:myisam/innodb");
        AKQuickBindScriptFunc("mysqlCol",&AKHandleMysql::mysqlCol,"mysqlCol(colType,colName,defaultVal);//定义表列");
        AKQuickBindScriptFunc("mysqlFormatTable",&AKHandleMysql::mysqlFormatTable,"mysqlFormatTable(cnn[,dbName]);//按照表定义，实例化所有数据表，指定dbName时只实例化该数据库中所有表");
        AKQuickBindScriptFunc("mysqlUpgradeTable",&AKHandleMysql::mysqlUpgradeTable,"mysqlUpgradeTable(cnn,defineDBName,defineTableName,dbName,tableName);//按照表定义defineDBName.defineTableName，升级或实例化数据表dbName.tableName");

        //数据表-字典同步API
        AKQuickBindScriptFunc("mysqlNewDict",&AKHandleMysql::mysqlNewDict,"rsID=mysqlNewDict(cnn,dbName,tableName);//新建表记录并返回记录ID");
        AKQuickBindScriptFunc("mysqlReadDict",&AKHandleMysql::mysqlReadDict,"dict=mysqlReadDict(cnn,dbName,tableName,selectExp);//表记录读到字典");
        AKQuickBindScriptFunc("mysqlUpdateFromDict",&AKHandleMysql::mysqlUpdateFromDict,"mysqlUpdateFromDict(cnn,dbName,tableName,dict);//字典更新到表记录");

    }

    //获取最后插入的记录ID
    bool AKHandleMysql::mysqlInsertID(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        out->setValue((double)mysql_insert_id(mysql));
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典更新到表记录
    bool AKHandleMysql::mysqlNewDict(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        std::string dbName=order->getParam(2)->toString();//数据库名
        std::string tableName=order->getParam(3)->toString();//表名

        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKDbInfoMap::iterator it=handler->mDBInfoMap.find(dbName);
        if(it==handler->mDBInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"没有定义数据库名 %s",dbName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        AKDBTableInfoMap &tableInfoMap=it->second;
        AKDBTableInfoMap::iterator it2=tableInfoMap.find(tableName);
        if(it2==tableInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"没有定义数据表名 %s.%s",dbName.c_str(),tableName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        AKDBTableInfo &tableInfo=it2->second;

        bool first=true;
        std::string sql="insert into `"+dbName+"`.`"+tableName+"` set ";
        AKDBColInfoMap::iterator it3=tableInfo.colInfoMap.begin();
        while(it3!=tableInfo.colInfoMap.end())//遍历所有列信息
        {
            const std::string &colName=it3->first;//列名
            AKDBColInfo &colInfo=it3->second;//列信息
            std::string &colType=colInfo[0];//列的类型
            if(colName!=""&&colType!="key"&&colType!="key64")//遇到列信息
            {
                std::string &defaultVal=colInfo[1];//列的默认值

                if(first)first=false;
                else sql+=",";
                sql+=colName;
                sql+="=";

                if(colType=="key"||colType=="key64"||colType=="int"||colType=="bigint")//int主键//整数
                {
                    sql+=defaultVal;
                }
                else if(colType=="varchar")//字符串（最大255字节）
                {
                    std::string destStr=defaultVal;
                    formatStr(destStr);//处理转义
                    sql+="'";
                    sql+=destStr;
                    sql+="'";
                }
                else if(colType=="tinytext"||colType=="text"||colType=="mediumtext"||colType=="longtext")//文本
                {
                    //sql+="''";
                    std::string destStr=defaultVal;
                    formatStr(destStr);//处理转义
                    sql+="'";
                    sql+=destStr;
                    sql+="'";
                }
                else if(colType=="tinyblob"||colType=="blob"||colType=="mediumblob"||colType=="longblob")//二进制数据（缓冲）
                {
                    sql+="''";
                }
                else if(colType=="datetime")//日期时间（YYYY-MM-DD HH:MM:SS）
                {
                    sql+="'";
                    sql+=defaultVal;
                    sql+="'";
                }
                else if(colType=="date")//日期（YYYY-MM-DD）
                {
                    sql+="'";
                    sql+=defaultVal;
                    sql+="'";
                }
            }
            ++it3;
        }

        int32 error=mysql_query(mysql,sql.c_str());
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            int8 tmp[1024*10];
            sprintf(tmp,"%s//%s",sql.c_str(),error);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }


        double id=mysql_insert_id(mysql);

        out->setValue((double)id);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //字典更新到表记录
    bool AKHandleMysql::mysqlUpdateFromDict(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        std::string dbName=order->getParam(2)->toString();//数据库名
        std::string tableName=order->getParam(3)->toString();//表名

        container::AKDict *dict=(container::AKDict *)order->getParam(4)->toHandle();//缓冲
        if(!script->getCore()->checkDict(dict))
        {
            onRunError(order,"无效字典");
            return false;//暂停执行脚本
        }

        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKDbInfoMap::iterator it=handler->mDBInfoMap.find(dbName);
        if(it==handler->mDBInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"没有定义数据库名 %s",dbName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        AKDBTableInfoMap &tableInfoMap=it->second;
        AKDBTableInfoMap::iterator it2=tableInfoMap.find(tableName);
        if(it2==tableInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"没有定义数据表名 %s.%s",dbName.c_str(),tableName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        AKDBTableInfo &tableInfo=it2->second;

        //构造更新sql
        std::string keyName;//主键名
        std::string keyVal;//主键值
        bool first=true;
        std::string sql="update `"+dbName+"`.`"+tableName+"` set ";
        AKDBColInfoMap::iterator it3=tableInfo.colInfoMap.begin();
        while(it3!=tableInfo.colInfoMap.end())//遍历所有列信息
        {
            const std::string &colName=it3->first;//列名
            AKDBColInfo &colInfo=it3->second;//列信息
            if(colName=="")//遇到主键信息
            {
                keyName=colInfo[0];//主键名
            }
            else//遇到列信息
            {
                std::string &colType=colInfo[0];//列的类型

                if(first)first=false;
                else sql+=",";
                sql+=colName;
                sql+="=";

                AKVar *var=dict->get(colName.c_str());
                if(colType=="key"||colType=="key64"||colType=="int"||colType=="bigint")//int主键//整数
                {
                    sql+=var->toString();

                    if(keyName==colName)//保存主键值
                    {
                        keyVal=var->toString();
                    }
                }
                else if(colType=="varchar")//字符串（最大255字节）
                {
                    std::string destStr=var->toString();
                    formatStr(destStr);//处理转义
                    sql+="'";
                    sql+=destStr;
                    sql+="'";
                }
                else if(colType=="tinytext"||colType=="text"||colType=="mediumtext"||colType=="longtext")//文本
                {
                    std::string destStr=var->toString();
                    formatStr(destStr);//处理转义
                    sql+="'";
                    sql+=destStr;
                    sql+="'";
                }
                else if(colType=="tinyblob"||colType=="blob"||colType=="mediumblob"||colType=="longblob")//二进制数据（缓冲）
                {
                    container::AKBuf *curBuf=(container::AKBuf *)var->toHandle();
                    if(!script->getCore()->checkBuf(curBuf))
                    {
                        onRunError(order,"无效buf");
                        return false;//暂停执行脚本
                    }

                    std::string hexStr;
                    AKCommon::buf2hex(hexStr,curBuf);

                    if(hexStr=="")sql+="''";
                    else
                    {
                        sql+="0x";
                        sql+=hexStr;
                    }
                }
                else if(colType=="datetime")//日期时间（YYYY-MM-DD HH:MM:SS）
                {
                    std::string destStr=var->toString();
                    formatStr(destStr);//处理转义
                    sql+="'";
                    sql+=destStr;
                    sql+="'";
                }
                else if(colType=="date")//日期（YYYY-MM-DD）
                {
                    std::string destStr=var->toString();
                    formatStr(destStr);//处理转义
                    sql+="'";
                    sql+=destStr;
                    sql+="'";
                }
            }
            ++it3;
        }
        sql+=" where `"+keyName+"`="+keyVal+" limit 1";
        int32 error=mysql_query(mysql,sql.c_str());
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            int8 tmp[1024*10];
            sprintf(tmp,"%s//%s",sql.c_str(),error);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //表记录读到字典
    bool AKHandleMysql::mysqlReadDict(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        std::string dbName=order->getParam(2)->toString();//数据库名
        std::string tableName=order->getParam(3)->toString();//表名
        std::string selectExp=order->getParam(4)->toString();//select查询表达式（接在where之后，例如：account='ak' and pwd='123'）

        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKDbInfoMap::iterator it=handler->mDBInfoMap.find(dbName);
        if(it==handler->mDBInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"没有定义数据库名 %s",dbName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        AKDBTableInfoMap &tableInfoMap=it->second;
        AKDBTableInfoMap::iterator it2=tableInfoMap.find(tableName);
        if(it2==tableInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"没有定义数据表名 %s.%s",dbName.c_str(),tableName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        AKDBTableInfo &tableInfo=it2->second;

        //构造查询sql
        bool first=true;
        std::string sql="select ";
        AKDBColInfoMap::iterator it3=tableInfo.colInfoMap.begin();
        while(it3!=tableInfo.colInfoMap.end())//遍历所有列信息
        {
            if(it3->first!="")
            {
                if(first)first=false;
                else sql+=",";
                const std::string &colName=it3->first;//列名
                sql+=colName;
            }
            ++it3;
        }
        sql+=" from `"+dbName+"`.`"+tableName+"` where "+selectExp+" limit 1";
        int32 error=mysql_query(mysql,sql.c_str());
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            int8 tmp[1024*10];
            sprintf(tmp,"%s//%s",sql.c_str(),error);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        MYSQL_RES *rs=mysql_store_result(mysql);
        MYSQL_ROW row=mysql_fetch_row(rs);

        //创建返回的字典
        container::AKDict *rtDict=getScript()->getCore()->createDict();

        if(row)//记录存在
        {
            //读取数值
            uint32 i=0;
            it3=tableInfo.colInfoMap.begin();
            while(it3!=tableInfo.colInfoMap.end())//遍历所有列信息
            {
                const std::string &colName=it3->first;//列名
                if(colName!="")
                {
                    AKDBColInfo &colInfo=it3->second;//列信息
                    std::string &colType=colInfo[0];//列的类型

                    if(colType=="key"||colType=="key64"||colType=="int"||colType=="bigint")//int主键//整数
                    {
                        rtDict->set(colName.c_str(),(double)atof(row[i]));
                    }
                    else if(colType=="varchar")//字符串（最大255字节）
                    {
                        rtDict->set(colName.c_str(),(const int8 *)row[i]);
                    }
                    else if(colType=="tinytext"||colType=="text"||colType=="mediumtext"||colType=="longtext")//文本
                    {
                        rtDict->set(colName.c_str(),(const int8 *)row[i]);
                    }
                    else if(colType=="tinyblob"||colType=="blob"||colType=="mediumblob"||colType=="longblob")//二进制数据（缓冲）
                    {
                        container::AKBuf *buf=getScript()->getCore()->createBuf();
                        unsigned long *lengths=mysql_fetch_lengths(rs);//列长度数组
                        const int8 *data=rs->current_row[i];
                        buf->write(data,lengths[i]);
                        buf->setPos(0);
                        rtDict->set(colName.c_str(),(void *)buf);
                    }
                    else if(colType=="datetime")//日期时间（YYYY-MM-DD HH:MM:SS）
                    {
                        rtDict->set(colName.c_str(),(const int8 *)row[i]);
                    }
                    else if(colType=="date")//日期（YYYY-MM-DD）
                    {
                        rtDict->set(colName.c_str(),(const int8 *)row[i]);
                    }
                    ++i;
                }
                ++it3;
            }
        }
        mysql_free_result(rs);


        out->setValue((void *)rtDict);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //根据tableNameMatch条件，获取表名数组
    bool AKHandleMysql::mysqlGetTables(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }
        std::string dbName=order->getParam(2)->toString();//数据库名
        std::string tableNameMatch=order->getParam(3)->toString();//匹配条件//='tableName'//like '%tableName%'//regexp '^tableName[0-9]+$'

        std::string sql="select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA='"+dbName+"' and TABLE_NAME "+tableNameMatch;
        int32 error=mysql_query(mysql,sql.c_str());
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            int8 tmp[1024*10];
            sprintf(tmp,"%s//%s",sql.c_str(),error);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        container::AKArr *tableArr=script->getCore()->createArr();
        MYSQL_RES *rs=mysql_store_result(mysql);
        MYSQL_ROW row;
        while(row=mysql_fetch_row(rs))
        {
            const int8 *tableName=rs->current_row[0];
            tableArr->push((const int8 *)tableName);
        }
        mysql_free_result(rs);

        out->setValue((void *)tableArr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //按照表定义，升级表格
    bool AKHandleMysql::mysqlUpgradeTable(AKScript *script,AKOrder *order)
    {
        if(!checkParams(5,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        const int8 *defineDBName=order->getParam(2)->toString();//表定义数据库名
        const int8 *defineTableName=order->getParam(3)->toString();//表定义表名
        const int8 *dbName=order->getParam(4)->toString();//数据库名
        const int8 *tableName=order->getParam(5)->toString();//表名

        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKDbInfoMap::iterator it=handler->mDBInfoMap.find(defineDBName);
        if(it==handler->mDBInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"表定义数据库名%s不存在",defineDBName);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        AKDBTableInfoMap &tableInfoMap=it->second;
        AKDBTableInfoMap::iterator it2=tableInfoMap.find(defineTableName);
        if(it2==tableInfoMap.end())
        {
            int8 tmp[1024*10];
            sprintf(tmp,"表定义表名%s不存在",defineTableName);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        AKDBTableInfo &tableInfo=it2->second;

        //创建列信息列表
        std::string keyName("");//主键名
        std::string keyInfo("");//主键信息
        AKStringList colInfoList;//列信息列表
        bool isTableExist=false;//检查表是否存在
        int8 errorStr[1024*10];
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //实例化表定义
        bool rt=formatTable(
                mysql,
                defineDBName,defineTableName,tableInfo,
                dbName,tableName,
                //返回
                keyName,keyInfo,colInfoList,isTableExist,errorStr
        );
        if(!rt)
        {
            onRunError(order,errorStr);
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    bool AKHandleMysql::formatTable(
            MYSQL *mysql,
            std::string defineDBName,//表定义数据库名
            std::string defineTableName,//表定义表名
            AKDBTableInfo &tableInfo,//表定义信息
            std::string dbName,//实例数据库名
            std::string tableName,//实例表名
            //返回
            std::string &keyName,//主键名
            std::string &keyInfo,//主键信息
            AKStringList &colInfoList,//列信息列表
            bool &isTableExist,//检查表是否存在
            int8 *errorStr//错误信息
    )
    {
        std::string info3="ENGINE="+tableInfo.engineName;
        info3+=" DEFAULT CHARSET=utf8;";//MyISAM表信息
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //检查表是否存在
        isTableExist=false;
        std::string sql="select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA='"+dbName+"' and TABLE_NAME='"+tableName+"' limit 1";
        int32 error=mysql_query(mysql,sql.c_str());
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            sprintf(errorStr,"%s//%s",sql.c_str(),error);
            return false;
        }
        MYSQL_RES *rs=mysql_store_result(mysql);
        MYSQL_ROW row=mysql_fetch_row(rs);
        if(row)//表存在
        {
            isTableExist=true;
        }
        mysql_free_result(rs);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        AKDBColInfoMap::iterator it2=tableInfo.colInfoMap.begin();
        while(it2!=tableInfo.colInfoMap.end())//遍历所有列信息
        {
            std::string colName=it2->first;//列名
            AKDBColInfo &colInfo=it2->second;//主键名信息
            if(colName=="")//遇到主键名信息
            {
                keyName=colInfo[0];//主键名
            }
            else//遇到列名
            {
                std::string &colType=colInfo[0];//列的类型
                std::string &defaultVal=colInfo[1];//列的默认值

                if(colType=="datetime"&&defaultVal=="")defaultVal="1970-1-1 00:00:00";
                else if(colType=="date"&&defaultVal=="")defaultVal="1970-1-1";

                //判断当前列是否是需要的列信息（如果列已经存在则不需要）
                bool isNeedColInfo=true;
                if(isTableExist)//表存在
                {
                    //判断列是否存在
                    std::string sql="select DATA_TYPE from information_schema.columns where table_schema='"+dbName+"' and table_name='"+tableName+"' and column_name='"+colName+"'";
                    int32 error=mysql_query(mysql,sql.c_str());
                    if(error)
                    {
                        const int8 *error=mysql_error(mysql);
                        sprintf(errorStr,"%s//%s",sql.c_str(),error);
                        return false;
                    }
                    MYSQL_RES *rs=mysql_store_result(mysql);
                    MYSQL_ROW row=mysql_fetch_row(rs);
                    if(row)//列存在
                    {
                        std::string curColType=row[0];
                        std::string cmpColType=colType;
                        if(cmpColType=="key")cmpColType="int";//key其实就是自增的int
                        else if(cmpColType=="key64")cmpColType="bigint";//key64其实就是自增的bigint
                        if(cmpColType!=curColType)//跟原列类型不一致
                        {
                            sprintf(errorStr,"表定义 %s.%s 中列 %s 类型 %s 与原来 %s 不一致",defineDBName.c_str(),defineTableName.c_str(),colName.c_str(),colType.c_str(),curColType.c_str());
                            return false;
                        }
                        isNeedColInfo=false;
                    }
                    mysql_free_result(rs);
                }

                if(isNeedColInfo)//是所需列信息
                {
                    //列信息
                    std::string info="";
                    info+="`";
                    info+=colName;
                    info+="` ";
                    if(colType=="key")//int主键，自增长
                    {
                        info+="int(11) not null AUTO_INCREMENT";
                        keyInfo="PRIMARY KEY (`"+colName+"`)";
                    }
                    else if(colType=="key64")//int大整数主键，自增长
                    {
                        info+="bigint(20) not null AUTO_INCREMENT";
                        keyInfo="PRIMARY KEY (`"+colName+"`)";
                    }
                    else if(colType=="int")//整数
                    {
                        info+="int(11) not null default '";
                        info+=defaultVal;
                        info+="'";
                    }
                    else if(colType=="bigint")//大整数
                    {
                        info+="bigint(20) not null default '";
                        info+=defaultVal;
                        info+="'";
                    }
                    else if(colType=="varchar")//字符串（最大255字节）
                    {
                        info+="varchar(255) not null default '";
                        info+=defaultVal;
                        info+="'";
                    }
                    else if(colType=="tinytext"||colType=="text"||colType=="mediumtext"||colType=="longtext")//文本
                    {
                        //tinytext		最大255字节
                        //text			最大65K字节
                        //mediumtext	最大16M字节
                        //longtext		最大4G字节
                        info+=colType;
                        //info+=" not null";
                    }
                    else if(colType=="tinyblob"||colType=="blob"||colType=="mediumblob"||colType=="longblob")//二进制数据（缓冲）
                    {
                        //tinyblob		最大255字节
                        //blob			最大65K字节
                        //mediumblob	最大16M字节
                        //longblob		最大4G字节
                        info+=colType;
                        //info+=" not null";
                    }
                    else if(colType=="datetime")//日期时间（YYYY-MM-DD HH:MM:SS）
                    {
                        info+=colType;
                        info+=" not null default '";
                        info+=defaultVal;
                        info+="'";
                    }
                    else if(colType=="date")//日期（YYYY-MM-DD）
                    {
                        info+=colType;
                        info+=" not null default '";
                        info+=defaultVal;
                        info+="'";
                    }
                    else
                    {
                        sprintf(errorStr,"表定义 %s.%s 中列 %s 类型错误 %s",defineDBName.c_str(),defineTableName.c_str(),colName.c_str(),colType.c_str());
                        return false;
                    }
                    colInfoList.push_back(info);
                }
            }
            ++it2;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(keyName=="")//没有主键名
        {
            sprintf(errorStr,"表定义 %s.%s 中没有主键",dbName.c_str(),tableName.c_str());
            return false;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(isTableExist)//表存在
        {
            if(colInfoList.size()>0)//存在需要添加的列信息
            {
                //添加表列
                std::string sql="alter table `"+dbName+"`.`"+tableName+"` \r\n";
                AKStringList::iterator it=colInfoList.begin();
                bool first=true;
                while(it!=colInfoList.end())
                {
                    if(first)first=false;
                    else sql+=",";

                    sql+="add ";
                    sql+=*it;
                    sql+="\r\n";
                    ++it;
                }
                //执行sql
                int32 error=mysql_query(mysql,sql.c_str());
                if(error)
                {
                    const int8 *error=mysql_error(mysql);
                    sprintf(errorStr,"%s//%s",sql.c_str(),error);
                    return false;
                }

            }
            //修改存储引擎
            std::string sql="alter table `"+dbName+"`.`"+tableName+"` ENGINE="+tableInfo.engineName;
            int32 error=mysql_query(mysql,sql.c_str());
            if(error)
            {
                const int8 *error=mysql_error(mysql);
                sprintf(errorStr,"%s//%s",sql.c_str(),error);
                return false;
            }
        }
        else//表不存在
        {
            //创建表
            std::string sql="CREATE TABLE `"+dbName+"`.`"+tableName+"` (\r\n";
            AKStringList::iterator it=colInfoList.begin();
            while(it!=colInfoList.end())
            {
                sql+=*it;
                sql+=",\r\n";
                ++it;
            }
            sql+=keyInfo+"\r\n";
            sql+=")"+info3;
            //执行sql
            int32 error=mysql_query(mysql,sql.c_str());
            if(error)
            {
                const int8 *error=mysql_error(mysql);
                sprintf(errorStr,"%s//%s",sql.c_str(),error);
                return false;
            }
        }
        return true;
    }

    //按照表定义，实例化所有数据表
    bool AKHandleMysql::mysqlFormatTable(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        //数据库名（指定dbName时只实例化该数据库中所有表）
        std::string targetDBName="";
        if(order->getParam(2))targetDBName=order->getParam(2)->toString();

        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKDbInfoMap::iterator it3=handler->mDBInfoMap.begin();
        while(it3!=handler->mDBInfoMap.end())//遍历数据库信息
        {
            std::string dbName=it3->first;//数据库名
            if(targetDBName==""||targetDBName==dbName)
            {
                AKDBTableInfoMap &tableInfoMap=it3->second;//数据表map
                AKDBTableInfoMap::iterator it=tableInfoMap.begin();
                while(it!=tableInfoMap.end())//遍历所有表信息
                {
                    std::string tableName=it->first;//表名

                    //创建列信息列表
                    std::string keyName("");//主键名
                    std::string keyInfo("");//主键信息
                    AKStringList colInfoList;//列信息列表
                    AKDBTableInfo &tableInfo=it->second;//表信息
                    bool isTableExist=false;//检查表是否存在
                    int8 errorStr[1024*10];
                    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //实例化表定义
                    bool rt=formatTable(
                            mysql,
                            dbName,tableName,tableInfo,
                            dbName,tableName,
                            //返回
                            keyName,keyInfo,colInfoList,isTableExist,errorStr
                    );
                    if(!rt)
                    {
                        onRunError(order,errorStr);
                        return false;//暂停执行脚本
                    }
                    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ++it;
                }
            }
            ++it3;
        }



        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //开始定义表
    bool AKHandleMysql::mysqlTable(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        script->getProcess()->mDBName=order->getParam(1)->toString();//数据库名
        script->getProcess()->mTableName=order->getParam(2)->toString();//表名
        if(order->getParam(3))
        {
            script->getProcess()->mEngineName=order->getParam(3)->toString();//存储引擎名（myisam/innodb）
        }
        else
        {
            script->getProcess()->mEngineName="myisam";
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //定义表列
    bool AKHandleMysql::mysqlCol(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        std::string colType=order->getParam(1)->toString();//数据类型
        std::string colName=order->getParam(2)->toString();//列名

        std::string &dbName=script->getProcess()->mDBName;
        std::string &tableName=script->getProcess()->mTableName;

        if(colName=="")
        {
            int8 tmp[1024*10];
            sprintf(tmp,"表定义 %s.%s 中列名不能为空",dbName.c_str(),tableName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        //默认值
        std::string defaultVal="";
        if(order->getParam(3))
        {
            defaultVal=order->getParam(3)->toString();//默认值
            formatStr(defaultVal);//处理转义
        }

        AKHandleMysql *handler=(AKHandleMysql *)getTopParent();
        AKDBTableInfoMap &tableInfoMap=handler->mDBInfoMap[dbName];//数据库信息
        AKDBTableInfo &tableInfo=tableInfoMap[tableName];//表信息
        tableInfo.engineName=script->getProcess()->mEngineName;//存储引擎名
        AKDBColInfo &colInfo=tableInfo.colInfoMap[colName];//列信息
        if(colInfo.size()>=2)
        {
            int8 tmp[1024*10];
            sprintf(tmp,"表定义 %s.%s 中列重名 %s",dbName.c_str(),tableName.c_str(),colName.c_str());
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }
        colInfo.push_back(colType);//列的数据类型
        colInfo.push_back(defaultVal);//列的默认值

        if(colType=="key"||colType=="key64")//主键
        {
            AKDBColInfo &colInfo=tableInfo.colInfoMap[""];//主键信息
            if(colInfo.size()>0)
            {
                int8 tmp[1024*10];
                sprintf(tmp,"表定义 %s.%s 中不能有多个主键",dbName.c_str(),tableName.c_str());
                onRunError(order,tmp);
                return false;//暂停执行脚本
            }
            colInfo.push_back(colName);//保存主键名
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断列类型是否为blob
    bool AKHandleMysql::mysqlIsBinCol(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        uint32 col=order->getParam(2)->toUint();
        uint32 cols=mysql_num_fields(rs);
        if(col>=cols)
        {
            onRunError(order,"无效列数");
            return false;//暂停执行脚本
        }

        enum_field_types colType=rs->fields[col].type;//获取列类型
        if(
                colType==MYSQL_TYPE_TINY_BLOB||
                colType==MYSQL_TYPE_MEDIUM_BLOB||
                colType==MYSQL_TYPE_LONG_BLOB||
                colType==MYSQL_TYPE_BLOB
                )//当前列类型为二进制数据
        {
            out->setValue((double)1);
        }
        else//其它类型
        {
            out->setValue((double)0);
        }

        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取记录集列的二进制数据
    bool AKHandleMysql::mysqlGetColBin(AKScript *script,AKOrder *order)
    {
        if(!checkParams(3,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        uint32 col=order->getParam(2)->toUint();
        uint32 cols=mysql_num_fields(rs);
        if(col>=cols)
        {
            onRunError(order,"无效列数");
            return false;//暂停执行脚本
        }

        //接收返回数据的buf
        container::AKBuf *buf=(container::AKBuf *)order->getParam(3)->toHandle();
        if(!script->getCore()->checkBuf(buf))
        {
            onRunError(order,"无效buf");
            return false;//暂停执行脚本
        }

        unsigned long *lengths=mysql_fetch_lengths(rs);//列长度数组

        const int8 *data=rs->current_row[col];
        buf->clear();
        buf->write(data,lengths[col]);
        buf->setPos(0);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取记录集列的字符串数据
    bool AKHandleMysql::mysqlGetColVal(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeString);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        uint32 col=order->getParam(2)->toUint();
        uint32 cols=mysql_num_fields(rs);
        if(col>=cols)
        {
            onRunError(order,"无效列数");
            return false;//暂停执行脚本
        }

        /*
        //因为mysql底层把text示为是blob，因此此段代码需要取消
        enum_field_types colType=rs->fields[col].type;//获取列类型
        if(
            colType==MYSQL_TYPE_TINY_BLOB||
            colType==MYSQL_TYPE_MEDIUM_BLOB||
            colType==MYSQL_TYPE_LONG_BLOB||
            colType==MYSQL_TYPE_BLOB
            )//当前列类型为blob
        {
            onRunError(order,"不能获取blob列数据");
            return false;//暂停执行脚本
        }
        */

        const int8 *val="";
        if(rs->current_row)
        {
            val=rs->current_row[col];
            if(!val)val="";
        }
        out->setValue((const int8 *)val);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取记录集下一行
    bool AKHandleMysql::mysqlNextRow(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        MYSQL_ROW row=mysql_fetch_row(rs);
        if(row)out->setValue((double)1);
        else out->setValue((double)0);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取记录集列名数组
    bool AKHandleMysql::mysqlGetColNames(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        container::AKArr *colNameArr=script->getCore()->createArr();

        MYSQL_FIELD *fd=NULL;
        while(fd=mysql_fetch_field(rs))
        {
            colNameArr->push(fd->name);
        }

        out->setValue((void *)colNameArr);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //获取记录集列数
    bool AKHandleMysql::mysqlGetCols(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        uint32 cols=mysql_num_fields(rs);

        out->setValue((double)cols);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //销毁mysql记录集
    bool AKHandleMysql::mysqlDestroyRs(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL_RES *rs=(MYSQL_RES *)order->getParam(1)->toHandle();
        if(!checkRs(rs))
        {
            onRunError(order,"无效mysql记录集句柄");
            return false;//暂停执行脚本
        }

        order->getParam(1)->setValue((void *)NULL);//这一句的作用：避免rs内存释放后，下次的AKRef类型创建时复用了刚才释放的内存地址，然后在脚本销毁时，rs变量指向了该AKRef内存地址，而造成引用计数释放的误操作
        destroyRs(rs);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //创建mysql记录集
    bool AKHandleMysql::mysqlCreateRs(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        const int8 *sql=order->getParam(2)->toString();
        int32 error=mysql_query(mysql,sql);
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            int8 tmp[1024*10];
            sprintf(tmp,"%s//%s",sql,error);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        MYSQL_RES *rs=createRs(mysql);

        out->setValue((void *)rs);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //执行mysql
    bool AKHandleMysql::mysqlExec(AKScript *script,AKOrder *order)
    {
        if(!checkParams(2,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        const int8 *sql=order->getParam(2)->toString();
        int32 error=mysql_query(mysql,sql);
        if(error)
        {
            const int8 *error=mysql_error(mysql);
            int8 tmp[1024*10];
            sprintf(tmp,"%s//%s",sql,error);
            onRunError(order,tmp);
            return false;//暂停执行脚本
        }

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //连接mysql
    bool AKHandleMysql::mysqlClose(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        destroyMysql(mysql);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //判断cnn是否断线，如果断线则会自动重连
    bool AKHandleMysql::mysqlPing(AKScript *script,AKOrder *order)
    {
        if(!checkParams(1,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeNumber);//设置返回类型

        MYSQL *mysql=(MYSQL *)order->getParam(1)->toHandle();
        if(!checkMysql(mysql))
        {
            onRunError(order,"无效mysql句柄");
            return false;//暂停执行脚本
        }

        mysql_ping(mysql);

        out->setValue((double)1);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }

    //连接mysql
    bool AKHandleMysql::mysqlCnn(AKScript *script,AKOrder *order)
    {
        if(!checkParams(4,script,order))return false;
        AKVar *out=order->getParam(0);//接收返回的变量
        out->setType(AKVar::typeHandle);//设置返回类型

        const int8 *ip=order->getParam(1)->toString();//域名或ip
        uint32 port=order->getParam(2)->toUint();//端口
        const int8 *usr=order->getParam(3)->toString();//mysql用户名
        const int8 *pwd=order->getParam(4)->toString();//mysql密码
        const int8 *db="";
        if(order->getParam(5))db=order->getParam(5)->toString();//mysql数据库名

        MYSQL *mysql=createMysql();

        //设置自动重连选项（需要通过调用mysql_ping进行判断，如果连接断开，会自动重连）
        char value=1;
        mysql_options(mysql,MYSQL_OPT_RECONNECT,(char *)&value);

        //设置编码
        //mysql_set_character_set(mysql,"utf8");
        mysql_options(mysql,MYSQL_SET_CHARSET_NAME,"utf8");

        //连接
        if(!mysql_real_connect(mysql,ip,usr,pwd,db,port,NULL,0))
        {
            const int8 *error=mysql_error(mysql);
            onRunError(order,error);
            return false;//暂停执行脚本
        }

        out->setValue((void *)mysql);
        script->nextOrder();//下一行指令
        return true;//继续执行脚本
    }


}

