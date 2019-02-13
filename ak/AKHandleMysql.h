//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKHANDLEMYSQL_H
#define AKSERVER_AKHANDLEMYSQL_H

#include "AKHandler.h"

namespace ak
{
    class AKScript;

    typedef AKStringArray AKDBColInfo;//列信息数组（[0]=数据类型;[1]=默认值）
    typedef std::map<std::string,AKDBColInfo> AKDBColInfoMap;//数据表信息（列名,数据列信息）//其中key=""的列保存着主键名
    struct AKDBTableInfo
    {
        std::string engineName;//存储引擎名
        AKDBColInfoMap colInfoMap;
    };
    typedef std::map<std::string,AKDBTableInfo> AKDBTableInfoMap;//数据表信息map（表名,数据表信息）
    typedef std::map<std::string,AKDBTableInfoMap> AKDbInfoMap;//数据库信息map（数据库名,数据表信息map）

    //内置mysql指令类
    class AKHandleMysql:public AKHandler
    {
    public:
        AKHandleMysql();
        ~AKHandleMysql();
        AKDefineCreate(AKHandleMysql);

        virtual void init(AKScript *script);
        virtual void reset();

    public:
        AKDefineScriptFunc(mysqlCnn);
        AKDefineScriptFunc(mysqlPing);
        AKDefineScriptFunc(mysqlClose);
        AKDefineScriptFunc(mysqlExec);
        AKDefineScriptFunc(mysqlCreateRs);
        AKDefineScriptFunc(mysqlDestroyRs);
        AKDefineScriptFunc(mysqlGetCols);
        AKDefineScriptFunc(mysqlNextRow);
        AKDefineScriptFunc(mysqlGetColNames);
        AKDefineScriptFunc(mysqlIsBinCol);
        AKDefineScriptFunc(mysqlGetColVal);
        AKDefineScriptFunc(mysqlGetColBin);
        AKDefineScriptFunc(mysqlInsertID);
        AKDefineScriptFunc(mysqlGetTables);

        AKDefineScriptFunc(mysqlUpgradeTable);
        AKDefineScriptFunc(mysqlFormatTable);
        AKDefineScriptFunc(mysqlTable);
        AKDefineScriptFunc(mysqlCol);

        AKDefineScriptFunc(mysqlNewDict);
        AKDefineScriptFunc(mysqlReadDict);
        AKDefineScriptFunc(mysqlUpdateFromDict);

    private:

        AKHandleBuf *getHandleBuf();

        MYSQL *createMysql();//创建mysql连接对象
        bool checkMysql(MYSQL *mysql);//检查mysql连接对象有效性
        void destroyMysql(MYSQL *mysql);//销毁mysql连接对象

        MYSQL_RES *createRs(MYSQL *mysql);//创建mysql记录集
        bool checkRs(MYSQL_RES *rs);//检查mysql记录集有效性
        void destroyRs(MYSQL_RES *rs);//销毁mysql记录集

        //实例化表定义
        bool formatTable(
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
        );

    private:
        AKMysqlMap mMysqlMap;
        AKRsMap mRsMap;
        AKHandleMysql *mParent;
        AKHandleBuf *mHandleBuf;

        AKDbInfoMap mDBInfoMap;
    };
}

#endif //AKSERVER_AKHANDLEMYSQL_H
