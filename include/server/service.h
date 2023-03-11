#ifndef SERVICE_H
#define SERVICE_H

#include "json.hpp"
#include "usermodel.h"
#include "offlinemodel.h"
#include "friendmodel.h"
#include "groupmodel.h"
#include "redis.h"
#include <string>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

using Msghandler = std::function<void(const TcpConnectionPtr &, json *, Timestamp)>;

class Service
{
public:
    static Service *getinstance();  //获取Service实例
    Msghandler gethandler(int msgid);   //获取msgid对应句柄

    void login_handler(const TcpConnectionPtr &, json *, Timestamp);  //登录业务
    void signup_handler(const TcpConnectionPtr &, json *, Timestamp); //注册业务
    void onechat_handler(const TcpConnectionPtr &, json *, Timestamp); //点对点聊天
    void addfriend_handler(const TcpConnectionPtr &, json *, Timestamp); //添加好友
    void creategroup_handler(const TcpConnectionPtr &, json *, Timestamp);  //创建群
    void addgroup_handler(const TcpConnectionPtr &, json *, Timestamp); //加入群组
    void groupchat_handler(const TcpConnectionPtr &, json *, Timestamp);  //群发消息
    void loginout_handler(const TcpConnectionPtr &, json *, Timestamp); //登出
    
    void redismsg_handler(int userid, const std::string &msg);
    void clientclose(const TcpConnectionPtr &); //客户端连接关闭
    void reset();   //重置业务
private:
    Service();
    Service(const Service &) = default;

    std::unordered_map<int, Msghandler> _handlers;      //业务句柄
    std::unordered_map<int, TcpConnectionPtr> _userconns;    //成功登录用户对应的连接    
    UserModel _usermodel;
    OfflineModel _offlinemodel;
    FriendModel _friendmodel;
    GroupModel _groupmodel;
    Redis _redis;
    std::mutex mtx;
};

#endif