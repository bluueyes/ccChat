#pragma once
#include "const.h"

class RedisConPool {

public:
    RedisConPool(size_t poolsize, const char* host, size_t port, const char* passwd)
        :_poolSize(poolsize),b_stop(false) {

        for (int i = 0;i < _poolSize;i++) {
            auto* context = redisConnect(host, port);
            if (context == nullptr || context->err != 0) {
                if (context != nullptr) {
                    redisFree(context);
                }
                continue;
            }

            auto* reply = (redisReply*)redisCommand(context, "AUTH %s", passwd);

            if (reply->type == REDIS_REPLY_ERROR) {
                std::cout << "认证失败" << std::endl;
                //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
                freeReplyObject(reply);
                continue;
            }

            // 执行成功 释放redisCommand执行后返回的redisReply所占用的内存
            freeReplyObject(reply);
            std::cout << "认证成功!" << std::endl;
            _connection.push(context);
        }

    }

    ~RedisConPool() {
        std::lock_guard<std::mutex> lock(_mutex);
        while (!_connection.empty()) {
            _connection.pop();
        }
    }

    redisContext* getConnection() {

        std::unique_lock<std::mutex> lock(_mutex);

        _con.wait(lock, [this]() {
            if (b_stop) {
                return true;
            }
            return !_connection.empty();
            });

        if (b_stop) {
            return nullptr;
        }

        auto* context = _connection.front();
        _connection.pop();
        return context;
    }

    void returnConnection(redisContext* context) {

        std::lock_guard<std::mutex> lock(_mutex);

        if (b_stop) {
            return;
        }

        _connection.push(context);

        _con.notify_one();
    }

    void Close() {
        b_stop = true;
        _con.notify_all();
    }

private:
    std::atomic<bool> b_stop;
    std::mutex _mutex;
    std::queue<redisContext*> _connection;
    std::condition_variable _con;
    size_t _poolSize;
};



class RedisMgr:public Singleton<RedisMgr>
{
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    bool Get(const std::string& key, std::string& value);
    bool Set(const std::string& key, const std::string& value);
    bool Auth(const std::string& password);
    bool LPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key, std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool HDel(const std::string key, const std::string field);
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string& key, const std::string& hkey);
    bool Del(const std::string& key);
    bool ExistsKey(const std::string& key);
    void Close();
private:
    RedisMgr();
    std::unique_ptr<RedisConPool> _con_pool;
};

