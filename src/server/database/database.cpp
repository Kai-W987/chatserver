#include "database.h"
#include <string>
#include <muduo/base/Logging.h>

using namespace muduo;

static std::string host = "127.0.0.1";
static std::string user = "root";
static std::string password = "root";
static std::string db = "chat";

Mysql::Mysql()
{
    _conn = mysql_init(nullptr);
}

Mysql::~Mysql()
{
    if (nullptr != _conn)
    {
        mysql_close(_conn);
    }
}

bool Mysql::connect()
{
    if (mysql_real_connect(_conn, host.c_str(), user.c_str(), password.c_str(), db.c_str(), 3306, nullptr, 0) != nullptr)
    {
        mysql_query(_conn, "set names gbk"); // 设置中文字符集
        return true;
    }
    LOG_INFO << "数据库连接失败";
    return false;
}

bool Mysql::update(const std::string &sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << "数据库更新失败:" << sql;
        return false;
    }
    return true;
}

MYSQL_RES* Mysql::query(const std::string &sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << "数据库操作失败:" << sql;
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL* Mysql::getconnect()
{
    return _conn;
}