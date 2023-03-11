#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <string>

class Mysql
{
public:
    Mysql();
    ~Mysql();
    bool connect();                        // 连接数据库
    bool update(const std::string &);      // 更新数据库
    MYSQL_RES *query(const std::string &); // 查询数据库
    MYSQL* getconnect();                   // 获取连接
private:
    MYSQL *_conn;
};

#endif