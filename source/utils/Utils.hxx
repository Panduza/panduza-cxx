#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

namespace Utils
{
    namespace Json
    {
        int ParseJson(const std::string &payload, json &data);
        bool KeyExists(const json &data, const std::string &key);
        int ToString(const json &data, const std::string &key, std::string &value);
        int ToInteger(const json &data, const std::string &key, int &value);
        int ToBool(const json &data, const std::string &key, bool &value);
        int ToObject(const json &data, const std::string &key, json &value);
        std::string GetTypeName(const json::value_t &type);
    };
    namespace String
    {
        bool StartsWith(const std::string &s, const std::string &prefix);
    }
};