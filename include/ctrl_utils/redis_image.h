#ifndef CTRL_UTILS_REDIS_IMAGES_H_
#define CTRL_UTILS_REDIS_IMAGES_H_

#include <algorithm>  // std::copy
#include <iostream>
#include <sstream>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <redis_client.h>
#include <string>
#include <cpp_redis/cpp_redis>

#include "ctrl_utils/string.h"
#include "ctrl_utils/opencv.h"

namespace ctrl_utils { 

    void RedisClient::mget_kinect(const std::vector<std::string>& image_keys,
                                  const std::vector<std::string>& keys, std::vector<cv::Mat>& out_images, std::vector<std::string>& out_values) {
        
        std::vector<std::string> concatenated_keys(image_keys.size() + keys.size());
        std::copy(image_keys.begin(), image_keys.end(), concatenated_keys.begin());
        std::copy(keys.begin(), keys.end(), concatenated_keys.begin() + image_keys.size());

        std::vector<std::string> values = sync_mget<std::string>(concatenated_keys);
        out_images.resize(image_keys.size());
        for (size_t i = 0; i < out_images.size(); i++) {
            out_images[i] = FromString<cv::Mat>(values[i]);
        }

        out_values.resize(keys.size());
        std::copy(values.begin() + image_keys.size(), values.end(), out_values.begin());

        // // Prepare key list
        // std::vector<const char *> argv = {"MGET"};
        // for (const auto& key : keys) {
        //     argv.push_back(key.c_str());
        // }
        // for (const auto& key : image_keys) {
        //     argv.push_back(key.c_str());
        // }

        return; 

        // // Call MGET command with variable argument formatting
        // redisReply *r = (redisReply *)redisCommandArgv(context_.get(), argv.size(), &argv[0], nullptr);
        // std::unique_ptr<redisReply, redisReplyDeleter> reply(r);

        // // Check for errors
        // if (!reply || reply->type != REDIS_REPLY_ARRAY)
        //     throw std::runtime_error("RedisClient: MGET command failed.");

        // cv::Mat buffer;
        // // Collect values
        // for (size_t i = 0; i < reply->elements; i++) {
        //     if (reply->element[i]->type != REDIS_REPLY_STRING)
        //         throw std::runtime_error("RedisClient: MGET command returned non-string values.");

        //     if (i<keys.size()) {
        //         values.push_back(reply->element[i]->str);
        //     } else {
        //         std::tuple<int, int, int> spec = image_specs[i-keys.size()];
        //         int height = std::get<0>(spec);
        //         int width = std::get<1>(spec);
        //         int type = std::get<2>(spec);
        //         cv::Mat buffer = cv::Mat(height, width, type, reply->element[i]->str);
        //         images.push_back(buffer.clone());
        //     }
        // }
        // return;
    }

    void RedisClient::mset_kinect(const std::vector<std::pair <std::string, cv::Mat> >& image_keyvals,
                                  const std::vector<std::pair <std::string, std::string> >& keyvals) {
        // Prepare key-value list

        std::vector<std::pair<std::string, std::string>> key_valstr();


        for (const auto& keyval : keyvals) {
     

            std::pair pair = std::make_pair(ToString(keyval.first),ToString(keyval.second));
            key_valstr.push_back(std::move(pair));

            // std::string s1; 
            // s= ToString(keyval.first);
            // argv.push_back(s);
            // s= ToString(keyval.second);


            // argv.push_back(s);
            //argv.push_back(keyval.first.c_str());
            //sizes.push_back(keyval.first.length());
            //argv.push_back(keyval.second.c_str());
            //sizes.push_back(keyval.second.length());
        }
        for (const auto& keyval : image_keyvals) {

            std::pair pair = std::make_pair(ToString(keyval.first),ToString<cv::Mat>(keyval.second));
            key_valstr.push_back(std::move(pair));

            // std::string s; 
            // s= ToString(keyval.first);
            // argv.push_back(s);
            // s= ToString<cv::Mat>(keyval.second);
            // argv.push_back(s);
            // argv.push_back(keyval.first.c_str());
            // sizes.push_back(keyval.first.length());
            // argv.push_back((const char*)keyval.second.data);
            // sizes.push_back(keyval.second.rows * keyval.second.cols * keyval.second.elemSize());
        }

        sync_mset(key_valstr); 

        // // Call MSET command with variable argument formatting
        // redisReply *r = (redisReply *)redisCommandArgv(context_.get(), argv.size(), &argv[0], sizes.data());
        // std::unique_ptr<redisReply, redisReplyDeleter> reply(r);

        // // Check for errors
        // if (!reply || reply->type == REDIS_REPLY_ERROR)
        //     throw std::runtime_error("RedisClient: MSET command failed.");

    }

}
#endif
