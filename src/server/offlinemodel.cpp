#include "offlinemodel.h"
#include "database.h"
#include <cstdio>

bool OfflineModel::insert(int id, std::string msg)
{
    char sql[1024] = {0};
    sprintf (sql, "insert into offlinemessage values(%d, '%s');", id, msg.c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        return mysql.update(sql);
    }

    return false;
}

bool OfflineModel::remove(int id)
{
    char sql[1024] = {0};
    sprintf (sql, "delete from offlinemessage where userid = %d;", id);

    Mysql mysql;
    if (mysql.connect())
    {
        return mysql.update(sql);
    }

    return false;
}

OfflineMsgList OfflineModel::query(int id)
{
    char sql[1024] = {0};
    sprintf (sql, "select message from offlinemessage where userid = %d;", id);

    OfflineMsgList list;

    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res;
        res = mysql.query(sql);
        if (res == nullptr) return list;

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            list.push_back(row[0]);
        }

        mysql_free_result(res);
    }

    return list;
}