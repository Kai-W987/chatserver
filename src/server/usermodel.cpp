#include "usermodel.h"
#include "database.h"
#include <string>
#include <cstdio>

bool UserModel::insert(User &user)
{
    // insert sql语句: insert into user(name, password, state) values(xx,xx, xx);
    std::string sql = "insert into user(name, password, state) values('" + user.getname() + "', '" + user.getpassword() + "', '" + user.getstate() + "');";

    Mysql mysql; // 数据库操作对象
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            int id = mysql_insert_id(mysql.getconnect());
            user.setid(id);
            return true;
        }
    }
    return false;
}

/****************************************************************
 * 描述：
 *      根据用户id查询用户信息，成功返回用户对象，失败返回空用户
 * 输入：
 *      id     -      用户id
 * 输出：
 *      查询成功返回用户对象，失败返回空用户
 ***************************************************************/
User UserModel::query(int id)
{
    // 查询sql
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d;", id);

    Mysql mysql; // 数据库操作对象
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            User user(atoi(row[0]), row[1], row[2], row[3]); // 返回查询到达user对象
            mysql_free_result(res);
            return user;
        }
    }

    return User(); // 返回空对象
}

bool UserModel::updatestate(User &user)
{
    // 更新sql
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d;", user.getstate().c_str(), user.getid());

    Mysql mysql; // 数据库操作对象
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }

    return false; // 返回空对象
}

void UserModel::resetstate()
{
    char sql[1024] = "update user set state = 'offline' where state = 'online';";

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}