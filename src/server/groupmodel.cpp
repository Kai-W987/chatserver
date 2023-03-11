#include "groupmodel.h"
#include "database.h"
#include <cstdio>
#include <cstring>

const std::string GroupModel::Creator = "creator";
const std::string GroupModel::Normal = "normal";

bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    sprintf (sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s');", group.getname().c_str(), group.getdesc().c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql)) 
        {
            group.setid(mysql_insert_id(mysql.getconnect()));
            return true;
        }
    }

    return false;
}

void GroupModel::addGroup(int userid, int groupid, std::string role)
{
    char sql[1024] = {0};
    sprintf (sql, "insert into groupuser values('%d', '%d', '%s');", groupid, userid, role.c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

std::vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    sprintf (sql, "select a.id,a.groupname,a.groupdesc from allgroup as a inner join groupuser as b on b.groupid = a.id where b.userid = %d;", userid);

    Mysql mysql;
    std::vector<Group> grouplist;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setid(atoi(row[0]));
                group.setname(row[1]);
                group.setdesc(row[2]);
                grouplist.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    for (Group &group : grouplist)
    {
        memset(sql, 0, 1024);
        sprintf (sql, "select a.id, a.name, a.state, b.grouprole from user as a inner join groupuser as b on b.userid = a.id where b.groupid = %d;", group.getid());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser groupuser;
                groupuser.setid(atoi(row[0]));
                groupuser.setname(row[1]);
                groupuser.setstate(row[2]);
                groupuser.setrole(row[3]);
                group.getusers().push_back(groupuser);
            }
            mysql_free_result(res);
        }
    }

    return grouplist;
}

std::vector<int> GroupModel::queryGroupOths(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf (sql, "select userid from groupuser where groupid = %d and userid != %d;", groupid, userid);

    Mysql mysql;
    std::vector<int> userlist;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                int id = atoi(row[0]);
                userlist.push_back(id);
            }
            mysql_free_result(res);
        }
    }

    return userlist;
}