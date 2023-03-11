#ifndef USER_H
#define USER_H

#include <string>

class User
{
public:
    User(int id = -1,
         std::string name = "",
         std::string password = "",
         const std::string &state = "offline")
        : _id(id), _name(name), _password(password), _state(state) {}

    ~User() {}

    void setid(int id) {_id = id;}
    void setname(std::string name) {_name = name;}
    void setpassword(std::string password) {_password = password;}
    void setstate(std::string state) {_state = state;}

    int getid() const {return _id;}
    std::string getname() const {return _name;}
    std::string getpassword() const {return _password;}
    std::string getstate() const {return _state;}

private:
    int _id;
    std::string _name;
    std::string _password;
    std::string _state;
};

#endif