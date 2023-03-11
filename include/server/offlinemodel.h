#ifndef OFFLINEMODEL_H
#define OFFLINEMODEL_H

#include <string>
#include <vector>

using OfflineMsgList = std::vector<std::string>;

class OfflineModel
{
public:
    bool insert(int, std::string);  //向数据库填入离线数据
    bool remove(int);    //从数据库移除某用户的离线数据
    OfflineMsgList query(int);       //从数据库中取出用户离线消息    
};

#endif