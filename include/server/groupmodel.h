#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>

class GroupModel
{
public:
    bool createGroup(Group &); //创建组
    void addGroup(int userid, int groupid, std::string role); //加入组
    std::vector<Group> queryGroups(int userid);  //查询用户所属的组
    std::vector<int> queryGroupOths(int userid, int groupid);    //查询一个组里其他用户id

    const static std::string Creator;
    const static std::string Normal;
};

#endif