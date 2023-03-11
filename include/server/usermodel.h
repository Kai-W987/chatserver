#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

class UserModel{
public:
    bool insert(User &);    //向数据库填入用户信息
    User query(int id);     //根据id查用户
    bool updatestate(User &);     //更新用户状态
    void resetstate();      //所有用户设置为离线
};

#endif