# chatserver
聊天服务器源码

基于muduo网络库实现网络模块，实现网络模块与业务模块分离

使用Mysql数据库存储用户数据

配置nginx负载均衡，实现服务器集群

基于redis发布-订阅模式，实现跨服务器通信

# 测试
可通过telnet测试服务器功能，通信样例如下：

注册
{"msgid":1, "name":"xxx", "password":"xxx"}

登录
{"msgid":3, "id":x, "password":"x"}

聊天
{"msgid":5, "id":A, "to":B, "msg":"hello"}

添加好友
{"msgid":6, "id":A, "friendid":B}

创建组
{"msgid":7, "id":A, "groupname":"xxx", "groupdesc":"xxx"}

加入群组
{"msgid":8, "id":A, "groupid":B}

发送群消息
{"msgid":9, "id":A, "groupid":B, "msg":"hello"}
