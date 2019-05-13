#ifndef CTRL_UTILS_REDIS_IMAGES_H_
#define CTRL_UTILS_REDIS_IMAGES_H_

#include <algorithm>  // std::copy
#include <iostream>
#include <sstream>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <string>
#include <cpp_redis/cpp_redis>

#include "ctrl_utils/redis_client.h"
#include "ctrl_utils/string.h"
#include "ctrl_utils/opencv.h"

namespace ctrl_utils {

  void RedisToKinect(const std::vector<std::string>& redis_values, size_t num_images,
                     std::vector<cv::Mat>& out_images, std::vector<std::string>& out_values) {

        out_images.resize(num_images);
        for (size_t i = 0; i < num_images; i++) {
            out_images[i] = FromString<cv::Mat>(redis_values[i]);
        }

        out_values.resize(redis_values.size() - num_images);
        std::copy(redis_values.begin() + num_images, redis_values.end(), out_values.begin());
        return;
    }


    std::vector<std::pair<std::string, std::string>> KinectToRedis(const std::vector<std::pair <std::string, cv::Mat> >& image_keyvals,
                                                                   const std::vector<std::pair <std::string, std::string> >& keyvals) {

        // Prepare key-value list
        std::vector<std::pair<std::string, std::string>> key_valstr;

        for (const auto& keyval : keyvals) {
            std::pair<std::string, std::string> pair = std::make_pair(ToString(keyval.first),ToString(keyval.second));
            key_valstr.push_back(std::move(pair));
        }
        for (const auto& keyval : image_keyvals) {
            std::pair<std::string, std::string> pair = std::make_pair(ToString(keyval.first),ToString<cv::Mat>(keyval.second));
            key_valstr.push_back(std::move(pair));
        }
        return key_valstr;
    }

} // namespace ctrl_utils

#endif
