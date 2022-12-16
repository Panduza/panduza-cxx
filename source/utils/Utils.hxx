#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

namespace Utils
{
    namespace Json
    {
        bool KeyExists(const json &data, const std::string &key);
        int ToString(const json &data, const std::string &key, std::string &value);
        int ToInteger(const json &data, const std::string &key, int &value);
        int ToBool(const json &data, const std::string &key, bool &value);
        int ToObject(const json &data, const std::string &key, json &value);
    };
    namespace String
    {
        bool EndsWith(std::string_view str, std::string_view suffix);
        bool StartsWith(std::string_view str, std::string_view prefix);
    }
};