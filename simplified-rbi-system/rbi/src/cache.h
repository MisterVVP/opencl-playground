#pragma once
#include <hiredis/hiredis.h>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <msgpack.hpp> // MessagePack library

namespace Cache {

    template <typename T>
    class ICache {
    public:
        virtual T get(const std::string& key) = 0;
        virtual void set(const std::string& key, const T& value, int expiration_seconds = 60) = 0;
        virtual std::vector<T> getVector(const std::string& key) = 0;
        virtual void setVector(const std::string& key, const std::vector<T>& value, int expiration_seconds = 60) = 0;
        virtual ~ICache() = default;
    };

    template <typename T>
    class RedisCache : public ICache<T> {
    private:
        redisContext* redis;

        void ensureConnected() {
            if (!redis || redis->err) {
                throw std::runtime_error("Redis connection error: " + std::string(redis ? redis->errstr : "Not connected"));
            }
        }

    public:
        RedisCache(const char* host, int port);
        ~RedisCache() override;

        T get(const std::string& key) override;
        void set(const std::string& key, const T& value, int expiration_seconds = 60) override;
        std::vector<T> getVector(const std::string& key) override;
        void setVector(const std::string& key, const std::vector<T>& value, int expiration_seconds = 60) override;
    };

    template <typename T>
    RedisCache<T>::RedisCache(const char* host, int port) {
        redis = redisConnect(host, port);
        if (!redis || redis->err) {
            throw std::runtime_error("Redis connection error: " + std::string(redis ? redis->errstr : "Unknown error"));
        }
    }

    template <typename T>
    RedisCache<T>::~RedisCache() {
        if (redis) {
            redisFree(redis);
        }
    }

    template <typename T>
    T RedisCache<T>::get(const std::string& key) {
        ensureConnected();

        redisReply* reply = static_cast<redisReply*>(redisCommand(redis, "GET %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Error executing Redis command");
        }

        T result;
        if (reply->type == REDIS_REPLY_STRING) {
            msgpack::unpacked unpacked;
            msgpack::unpack(unpacked, reply->str, reply->len);
            unpacked.get().convert(result);
        } else if (reply->type == REDIS_REPLY_NIL) {
            result = T(); // Return default value of T
        } else {
            freeReplyObject(reply);
            throw std::runtime_error("Unexpected Redis reply type");
        }

        freeReplyObject(reply);
        return result;
    }

    template <typename T>
    void RedisCache<T>::set(const std::string& key, const T& value, int expiration_seconds) {
        ensureConnected();

        msgpack::sbuffer buffer;
        msgpack::pack(buffer, value);

        redisReply* reply = static_cast<redisReply*>(redisCommand(redis, "SET %s %b EX %d", 
            key.c_str(), buffer.data(), buffer.size(), expiration_seconds));
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            throw std::runtime_error("Error executing Redis command");
        }

        freeReplyObject(reply);
    }

    template <typename T>
    std::vector<T> RedisCache<T>::getVector(const std::string& key) {
        ensureConnected();

        std::vector<T> result;
        redisReply* reply = static_cast<redisReply*>(redisCommand(redis, "HGETALL %s", key.c_str()));

        if (!reply) {
            throw std::runtime_error("Failed to execute Redis command.");
        }

        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 1; i < reply->elements; i += 2) { // Skip keys, take values
                T value;
                msgpack::unpacked unpacked;
                msgpack::unpack(unpacked, reply->element[i]->str, reply->element[i]->len);
                unpacked.get().convert(value);
                result.push_back(value);
            }
        } else if (reply->type != REDIS_REPLY_NIL) {
            freeReplyObject(reply);
            throw std::runtime_error("Unexpected Redis reply type");
        }

        freeReplyObject(reply);
        return result;
    }

    template <typename T>
    void RedisCache<T>::setVector(const std::string& key, const std::vector<T>& value, int expiration_seconds) {
        ensureConnected();

        for (size_t i = 0; i < value.size(); ++i) {
            std::ostringstream field;
            field << "index_" << i;

            msgpack::sbuffer buffer;
            msgpack::pack(buffer, value[i]);

            redisReply* reply = static_cast<redisReply*>(redisCommand(redis, "HSET %s %s %b",
                key.c_str(), field.str().c_str(), buffer.data(), buffer.size()));
            if (!reply) {
                throw std::runtime_error("Failed to execute Redis command.");
            }

            freeReplyObject(reply);
        }

        redisReply* reply = static_cast<redisReply*>(redisCommand(redis, "EXPIRE %s %d", key.c_str(), expiration_seconds));
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            throw std::runtime_error("Failed to set expiration time.");
        }

        freeReplyObject(reply);
    }

} // namespace Cache
