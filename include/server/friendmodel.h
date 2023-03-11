#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>

using Friendlist = std::vector<User>;

class FriendModel
{
public:
    void insert(int, int);  //向friend表加入关系
    Friendlist query(int);    //查询用户的好友列表
};

#endif