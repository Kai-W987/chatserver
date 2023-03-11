#include "service.h"
#include "public.h"
#include <string>
#include <vector>
#include <functional>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace std::placeholders;

Service::Service()
{
    //添加业务句柄
    _handlers.insert({LOGIN_MSG, std::bind(&Service::login_handler, this, _1, _2, _3)});
    _handlers.insert({SIGNUP_MSG, std::bind(&Service::signup_handler, this, _1, _2, _3)});
    _handlers.insert({ONE_CHAT_MSG, std::bind(&Service::onechat_handler, this, _1, _2, _3)});
    _handlers.insert({ADD_FRIEND_MSG, std::bind(&Service::addfriend_handler, this, _1, _2, _3)});
    _handlers.insert({CREATE_GROUP_MSG, std::bind(&Service::creategroup_handler, this, _1, _2, _3)});
    _handlers.insert({ADD_GROUP_MSG, std::bind(&Service::addgroup_handler, this, _1, _2, _3)});
    _handlers.insert({GOURP_CHAT_MSG, std::bind(&Service::groupchat_handler, this, _1, _2, _3)});
    _handlers.insert({LOGIN_OUT_MSG, std::bind(&Service::loginout_handler, this, _1, _2, _3)});

    //连接redis，注册消息处理函数
    if (_redis.connect())
    {
        _redis.init_notify_handler(std::bind(&Service::redismsg_handler, this, _1, _2));
    }
}

Service *Service::getinstance()
{
    static Service service;
    return &service;
}

Msghandler Service::gethandler(int msgid)
{
    auto ite = _handlers.find(msgid);
    if (ite == _handlers.end())
    {
        return [=](const TcpConnectionPtr &, json *, Timestamp)
        {
            LOG_ERROR << "msgid:" << msgid << " not found\n";
        };
    }
    else
    {
        return _handlers[msgid];
    }
}

// 登录
void Service::login_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    LOG_INFO << "login_handler\n";
    int id = (*js)["id"];
    std::string password = (*js)["password"];

    User user = _usermodel.query(id);
    if (user.getid() != -1 && user.getpassword() == password) // 确认身份
    {
        if (user.getstate() == "online") // 重复登录
        {
            json js;
            js["msgid"] = LOGIN_MSG_ACK;
            js["errno"] = 2;
            js["errmsg"] = "重复登录";
            conn->send(js.dump());
        }
        else
        {
            // 记录用户连接
            {
                std::lock_guard<std::mutex> lck(mtx);
                _userconns[id] = conn;
            }

            // 更新用户状态
            user.setstate("online");
            _usermodel.updatestate(user);

            //redis订阅通道
            _redis.subscribe(user.getid());

            // 推送用户信息
            json js;
            js["msgid"] = LOGIN_MSG_ACK;
            js["errno"] = 0;
            js["id"] = id;
            js["name"] = user.getname();

            // 离线消息
            OfflineMsgList offmsglist = _offlinemodel.query(id);
            if (!offmsglist.empty())
            {
                js["offlinemsg"] = offmsglist;
                _offlinemodel.remove(id);
            }

            // 好友列表
            Friendlist friendlist = _friendmodel.query(id);
            if (!friendlist.empty())
            {
                std::vector<std::string> res;
                for (User &user : friendlist)
                {
                    json j;
                    j["id"] = user.getid();
                    j["name"] = user.getname();
                    j["state"] = user.getstate();
                    res.push_back(j.dump());
                }
                js["friendlist"] = res;
            }

            //群组信息
            std::vector<Group> grouplist = _groupmodel.queryGroups(id);
            if (!grouplist.empty())
            {
                std::vector<std::string> groups;    //组列表
                for (Group &group : grouplist)      
                {
                    json gjs; //序列号组信息
                    gjs["groupid"] = group.getid();
                    gjs["groupname"] = group.getname();
                    gjs["groupdesc"] = group.getdesc();
                    std::vector<std::string> groupusers;    //组成员列表
                    for (GroupUser &groupuser : group.getusers())
                    {
                        json gujs;  //序列号组成员信息
                        gujs["id"] = groupuser.getid();
                        gujs["name"] = groupuser.getname();
                        gujs["state"] = groupuser.getstate();
                        gujs["role"] = groupuser.getrole();
                        groupusers.push_back(gujs.dump());
                    }
                    gjs["groupusers"] = groupusers;
                    groups.push_back(gjs.dump());
                }
                js["groups"] = groups;
            }

            conn->send(js.dump());
        }
    }
    else
    {
        // 密码或id错误
        json js;
        js["msgid"] = LOGIN_MSG_ACK;
        js["errno"] = 1;
        js["errmsg"] = "密码或id错误";
        conn->send(js.dump());
    }
}

// 注册
void Service::signup_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    LOG_INFO << "signup_handler\n";
    std::string name = (*js)["name"];
    std::string password = (*js)["password"];

    User user(-1, name, password);

    if (_usermodel.insert(user))
    {
        // 注册成功: 返还响应信息
        json r;
        r["msgid"] = SINGUP_MSG_ACK;
        r["errno"] = 0;
        r["id"] = user.getid();
        conn->send(r.dump());
    }
    else
    {
        // 注册失败
        json r;
        r["msgid"] = SINGUP_MSG_ACK;
        r["errno"] = 1;
        r["errmsg"] = "注册失败";
        conn->send(r.dump());
    }
}

// 点对点聊天
void Service::onechat_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int toid = (*js)["to"];

    {
        std::lock_guard<std::mutex> lck(mtx);
        auto ite = _userconns.find(toid);
        if (ite != _userconns.end())
        {
            // 对方在线
            ite->second->send(js->dump());
            return;
        }
    }

    //对方不在当前服务器连接，查询是否在其他服务器上线
    User oth = _usermodel.query(toid);
    if (oth.getstate() == "online")
    {
        _redis.publish(toid, js->dump());   //向对方通道发布消息
    }
    else 
    {
        // 对方离线
        _offlinemodel.insert(toid, js->dump()); // 存入数据库
    }
}

// 添加好友
void Service::addfriend_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    int friendid = (*js)["friendid"];
    _friendmodel.insert(userid, friendid); // 添加好友关系
}

// 创建群组
void Service::creategroup_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    std::string groupname = (*js)["groupname"];
    std::string groupdesc = (*js)["groupdesc"];

    Group group(-1, groupname, groupdesc);
    if (_groupmodel.createGroup(group))
    {
        _groupmodel.addGroup(userid, group.getid(), GroupModel::Creator);
    }
}

// 加入群组
void Service::addgroup_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    int groupid = (*js)["groupid"];
    _groupmodel.addGroup(userid, groupid, GroupModel::Normal);
}

// 群发消息
void Service::groupchat_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    int groupid = (*js)["groupid"];

    std::vector<int> groupusersid = _groupmodel.queryGroupOths(userid, groupid);
    std::lock_guard<std::mutex> lck(mtx);
    for (int id : groupusersid)
    {
        auto ite = _userconns.find(id);
        if (ite != _userconns.end())
        {
            ite->second->send(js->dump());
        }
        else
        {
            //查询是否在其他服务器登录
            User oth = _usermodel.query(id);
            if (oth.getstate() == "online")
            {
                _redis.publish(id, js->dump());
            }
            else
            {
                _offlinemodel.insert(id, js->dump());
            }
        }
    }
}

//用户登出
void Service::loginout_handler(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int id = (*js)["id"];
    User user(id);
    user.setstate("offline");
    {
        std::lock_guard<std::mutex> lck(mtx);
        if (_userconns.find(id) != _userconns.end()) _userconns.erase(id);
    }
    _redis.unsubscribe(id);
    _usermodel.updatestate(user);
}

//redis通道消息处理函数
void Service::redismsg_handler(int userid, const std::string &msg)
{
    User user = _usermodel.query(userid);
    
    {
        std::lock_guard<std::mutex> lck(mtx);
        auto ite = _userconns.find(user.getid());
        if (ite != _userconns.end())
        {
            ite->second->send(msg);
            return;
        }
    }

    if (user.getid() != -1)
    {
        _offlinemodel.insert(userid, msg);
    }
}

// 客户端关闭，处理用户状态
void Service::clientclose(const TcpConnectionPtr &conn)
{
    User user;

    // 剔除_userconns里对应的连接
    {
        std::lock_guard<std::mutex> lck(mtx);
        for (auto ite = _userconns.begin(); ite != _userconns.end(); ++ite)
        {
            if (ite->second == conn)
            {
                user.setid(ite->first);
                _userconns.erase(ite);
                break;
            }
        }
    }

    if (user.getid() != -1)
    {
        _redis.subscribe(user.getid());
        user.setstate("offline");
        _usermodel.updatestate(user);
    }
}

void Service::reset()
{
    _usermodel.resetstate();
}
