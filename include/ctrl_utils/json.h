/**
 * json.h
 *
 * Copyright 2019. All Rights Reserved.
 *
 * Created: January 31, 2019
 * Authors: Toki Migimatsu
 */

#ifndef CTRL_UTILS_JSON_H_
#define CTRL_UTILS_JSON_H_

#include <exception>  // std::runtime_error
#include <nlohmann/json.hpp>

#include "eigen.h"
#include "string.h"

namespace ctrl_utils {

template <>
inline std::string ToString(const nlohmann::json& value) {
  return value.dump();
}

template <>
inline nlohmann::json FromString(const std::string& str) {
  return nlohmann::json::parse(str);
}

}  // namespace ctrl_utils

namespace Eigen {

template <typename Derived>
void to_json(nlohmann::json& json, const Eigen::DenseBase<Derived>& matrix) {
  json = nlohmann::json::array();

  if (matrix.cols() == 1) {
    for (int i = 0; i < matrix.rows(); i++) {
      json.push_back(matrix(i));
    }
    return;
  }

  for (int i = 0; i < matrix.rows(); i++) {
    json.emplace_back(nlohmann::json::array());
    nlohmann::json& json_i = json[i];
    for (int j = 0; j < matrix.cols(); j++) {
      json_i.push_back(matrix(i, j));
    }
  }
}

template <typename Derived>
void from_json(const nlohmann::json& json, Eigen::DenseBase<Derived>& matrix) {
  if (json.type() != nlohmann::json::value_t::array) {
    throw std::runtime_error("Eigen::from_json(): Json type is not an array.");
  }

  if (json.empty() ||
      (json[0].type() == nlohmann::json::value_t::array && json[0].empty())) {
    if (matrix.size() != 0) {
      throw std::runtime_error("Eigen::from_json(): Json array is empty.");
    }
    return;
  }

  const int kNumRows = json.size();
  if (json[0].type() != nlohmann::json::value_t::array) {
    if (matrix.size() == 0) {
      matrix.resize(kNumRows);
    } else if (matrix.rows() != kNumRows) {
      throw std::runtime_error(
          "Eigen::from_json(): Json array is not the same size.");
    }
    for (int i = 0; i < kNumRows; i++) {
      matrix(i) = json[i];
    }
    return;
  }

  const int kNumCols = json[0].size();
  if (matrix.size() == 0) {
    matrix.resize(kNumRows, kNumCols);
  } else if (matrix.rows() != kNumRows || matrix.cols() != kNumCols) {
    throw std::runtime_error(
        "Eigen::from_json(): Json array is not the same size.");
  }

  for (int i = 0; i < kNumRows; i++) {
    const nlohmann::json& json_i = json[i];
    for (int j = 0; j < kNumCols; j++) {
      matrix(i, j) = json_i[j];
    }
  }
}

template <typename Derived>
void to_json(nlohmann::json& json, const Eigen::QuaternionBase<Derived>& quat) {
  json["w"] = quat.w();
  json["x"] = quat.x();
  json["y"] = quat.y();
  json["z"] = quat.z();
}

template <typename Derived>
void from_json(const nlohmann::json& json,
               Eigen::QuaternionBase<Derived>& quat) {
  quat.w() = json["w"].get<typename Derived::Scalar>();
  quat.x() = json["x"].get<typename Derived::Scalar>();
  quat.y() = json["y"].get<typename Derived::Scalar>();
  quat.z() = json["z"].get<typename Derived::Scalar>();
  quat.normalize();
}

inline void to_json(nlohmann::json& json, const Eigen::Isometry3d& T) {
  json["pos"] = T.translation();
  json["ori"] = Eigen::Quaterniond(T.linear());
}

inline void from_json(const nlohmann::json& json, Eigen::Isometry3d& T) {
  std::vector<double> temp_pos = json["pos"].get<std::vector<double>>();
  Eigen::Vector3d pos(temp_pos[0], temp_pos[1], temp_pos[2]);
  std::vector<double> temp_ori = json["ori"].get<std::vector<double>>();
  Eigen::Quaterniond ori(temp_ori[0], temp_ori[1], temp_ori[2], temp_ori[3]);
  T = Eigen::Translation3d(pos) * ori;
}

}  // namespace Eigen

#endif  // CTRL_UTILS_JSON_H_
