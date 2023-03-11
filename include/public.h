#ifndef PUBLIC_H
#define PUBLIC_H

enum Msgid{
    SIGNUP_MSG = 1,  //注册信息
    SINGUP_MSG_ACK,  //注册响应
    LOGIN_MSG,       //登录信息
    LOGIN_MSG_ACK,   //登录响应
    ONE_CHAT_MSG,    //单人聊天
    ADD_FRIEND_MSG,   //添加好友
    CREATE_GROUP_MSG,   //创建群
    ADD_GROUP_MSG,      //加入群
    GOURP_CHAT_MSG,      //群发消息
    LOGIN_OUT_MSG       //退出消息
};

#endif