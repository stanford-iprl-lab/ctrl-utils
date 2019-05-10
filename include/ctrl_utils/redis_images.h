#ifndef CTRL_UTILS_REDIS_IMAGES_H_
#define CTRL_UTILS_REDIS_IMAGES_H_


#include <iostream>
#include <sstream>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <redis_client.h>
#include <string>
#include <cpp_redis/cpp_redis>

#include "ctrl_utils/string.h"

void RedisClient::getImage(const std::string& key, std::tuple<int, int, int> spec, cv::Mat& value) {
    // Call GET command
    auto reply = command("GET %s", key.c_str());

    // Check for errors
    if (!reply || reply->type == REDIS_REPLY_ERROR || reply->type == REDIS_REPLY_NIL)
        throw std::runtime_error("RedisClient: GET '" + key + "' failed.");
    if (reply->type != REDIS_REPLY_STRING)
        throw std::runtime_error("RedisClient: GET '" + key + "' returned non-string value.");

    // Return value
    int height = std::get<0>(spec);
    int width = std::get<1>(spec);
    int type = std::get<2>(spec);
    cv::Mat buffer = cv::Mat(height, width, type, reply->str);
    buffer.copyTo(value);
    return;
}


void RedisClient::setImage(const std::string& key, const cv::Mat& value) {
    int size = value.rows * value.cols * value.elemSize();
    // Call SET command
    auto reply = command("SET %s %b", key.c_str(), value.data, size);

    // Check for errors
    if (!reply || reply->type == REDIS_REPLY_ERROR)
        throw std::runtime_error("RedisClient: SET '" + key + "' failed.");
}




void RedisClient::mget_kinect(const std::vector<std::string>& image_keys, std::vector<std::tuple<int, int, int>> image_specs,
                              const std::vector<std::string>& keys, std::vector<cv::Mat>& images, std::vector<std::string>& values) {
    // Prepare key list
    std::vector<const char *> argv = {"MGET"};
    for (const auto& key : keys) {
        argv.push_back(key.c_str());
    }
    for (const auto& key : image_keys) {
        argv.push_back(key.c_str());
    }

    // Call MGET command with variable argument formatting
    redisReply *r = (redisReply *)redisCommandArgv(context_.get(), argv.size(), &argv[0], nullptr);
    std::unique_ptr<redisReply, redisReplyDeleter> reply(r);

    // Check for errors
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        throw std::runtime_error("RedisClient: MGET command failed.");

    cv::Mat buffer;
    // Collect values
    for (size_t i = 0; i < reply->elements; i++) {
        if (reply->element[i]->type != REDIS_REPLY_STRING)
            throw std::runtime_error("RedisClient: MGET command returned non-string values.");

        if (i<keys.size()) {
            values.push_back(reply->element[i]->str);
        } else {
            std::tuple<int, int, int> spec = image_specs[i-keys.size()];
            int height = std::get<0>(spec);
            int width = std::get<1>(spec);
            int type = std::get<2>(spec);
            cv::Mat buffer = cv::Mat(height, width, type, reply->element[i]->str);
            images.push_back(buffer.clone());
        }
    }
    return;
}

void RedisClient::mset_kinect(const std::vector<std::pair <std::string, cv::Mat> >& image_keyvals,
                              const std::vector<std::pair <std::string, std::string> >& keyvals) {
    // Prepare key-value list
    std::vector<const char *> argv = {"MSET"};
    std::vector<size_t> sizes = {4};
    for (const auto& keyval : keyvals) {
        argv.push_back(keyval.first.c_str());
        sizes.push_back(keyval.first.length());
        argv.push_back(keyval.second.c_str());
        sizes.push_back(keyval.second.length());
    }
    for (const auto& keyval : image_keyvals) {
        argv.push_back(keyval.first.c_str());
        sizes.push_back(keyval.first.length());
        argv.push_back((const char*)keyval.second.data);
        sizes.push_back(keyval.second.rows * keyval.second.cols * keyval.second.elemSize());
    }

    // Call MSET command with variable argument formatting
    redisReply *r = (redisReply *)redisCommandArgv(context_.get(), argv.size(), &argv[0], sizes.data());
    std::unique_ptr<redisReply, redisReplyDeleter> reply(r);

    // Check for errors
    if (!reply || reply->type == REDIS_REPLY_ERROR)
        throw std::runtime_error("RedisClient: MSET command failed.");

}


#endif

