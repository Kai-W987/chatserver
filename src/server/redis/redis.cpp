#include "redis.h"
#include <muduo/base/Logging.h>

using namespace muduo;

Redis::Redis()
    : _publish_context(nullptr), _subcribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }

    if (_subcribe_context != nullptr)
    {
        redisFree(_subcribe_context);
    }
}

bool Redis::connect()
{
    // 负责publish发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context)
    {
        LOG_INFO << "connect redis failed!";
        return false;
    }

    // 负责subscribe订阅消息的上下文连接
    _subcribe_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _subcribe_context)
    {
        LOG_INFO << "connect redis failed!";
        return false;
    }

    // 在单独的线程中，监听通道上的事件，有消息给业务层进行上报
    thread t([&]() {
        observer_channel_message();
    });
    t.detach();

    return true;
}

// 向redis指定的通道channel发布消息
bool Redis::publish(int channel, string message)
{
    LOG_INFO << "redis publish to channel: " << channel;
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (nullptr == reply)
    {
        LOG_INFO << "publish failed!";
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel)
{
    //只发布订阅命令，不阻塞等待结果
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "SUBSCRIBE %d", channel))
    {
        LOG_INFO << "subscribe failed!";
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done))
        {
            LOG_INFO << "subscribe failed!";
            return false;
        }
    }

    return true;
}

// 向redis指定的通道unsubscribe取消订阅消息
bool Redis::unsubscribe(int channel)
{
    //只发布取消订阅命令，不阻塞等待结果
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "UNSUBSCRIBE %d", channel))
    {
        LOG_INFO << "unsubscribe failed!";
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done))
        {
            LOG_INFO << "unsubscribe failed!";
            return false;
        }
    }
    return true;
}

// 在独立线程中接收订阅通道中的消息
void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subcribe_context, (void **)&reply))
    {
        // "title" "channel" "msg"
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            // 给业务层上报通道上发生的消息
            _notify_message_handler(atoi(reply->element[1]->str) , reply->element[2]->str);
        }

        freeReplyObject(reply);
    }

}

void Redis::init_notify_handler(function<void(int,string)> fn)
{
    this->_notify_message_handler = fn;
}