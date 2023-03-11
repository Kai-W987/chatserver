#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"
#include <string>

class GroupUser :public User
{
public:
    void setrole(std::string role) {_role = role;}
    std::string getrole() const {return _role;}

private:
    std::string _role;
};

#endif