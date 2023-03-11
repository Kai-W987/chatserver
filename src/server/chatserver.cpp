#include "chatserver.h"
#include "json.hpp"
#include "service.h"
#include <string>
#include <functional>

using namespace std::placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg),
      _loop(loop)
{
    // 输出连接信息
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 输出读写事件信息
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected()) {
        Service::getinstance()->clientclose(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    //json反序列
    std::string str = buffer->retrieveAllAsString();
    json js = json::parse(str);
    //根据msg_id字段调用句柄
    auto handler = Service::getinstance()->gethandler(js["msgid"].get<int>());
    handler(conn, &js, time);
}