#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace pza
{
    namespace utils
    {
        namespace json
        {
            int ParseJson(const std::string &payload, nlohmann::json &data);
            bool KeyExists(const nlohmann::json &data, const std::string &key);
            int ToString(const nlohmann::json &data, const std::string &key, std::string &value);
            int ToInteger(const nlohmann::json &data, const std::string &key, int &value);
            int ToBool(const nlohmann::json &data, const std::string &key, bool &value);
            int ToObject(const nlohmann::json &data, const std::string &key, nlohmann::json &value);
            std::string GetTypeName(const nlohmann::json::value_t &type);
        };
        namespace string
        {
            bool StartsWith(const std::string &s, const std::string &prefix);
        }
    };
};