#ifndef GOURP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>

class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
            :_id(id),
             _name(name),
             _desc(desc)
    {}

    void setid(int id) {_id = id;}
    void setname(std::string name) {_name = name;}
    void setdesc(std::string desc) {_desc = desc;}

    int getid() const {return _id;}
    std::string getname() const {return _name;}
    std::string getdesc() const {return _desc;}
    std::vector<GroupUser> getusers() const {return _groupusers;}

private:
    int _id;             //组序号
    std::string _name;   //组名
    std::string _desc;   //组功能
    std::vector<GroupUser> _groupusers;  //组用户
};

#endif