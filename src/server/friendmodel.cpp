#include "friendmodel.h"
#include "database.h"
#include <cstdio>

void FriendModel::insert(int userid, int friendid)  
{
    char sql[1024] = {0};
    sprintf (sql, "insert into friend values(%d, %d);", userid, friendid);

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

Friendlist FriendModel::query(int userid)
{
    char sql[1024] = {0};
    sprintf (sql, "select a.id,a.name,a.state from user as a inner join friend as b on b.friendid = a.id where b.userid = %d;", userid);

    Friendlist list;
    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res;
        res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);
                list.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return list;
}