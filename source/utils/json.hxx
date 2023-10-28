#pragma once

#include <nlohmann/json.hpp>

namespace json
{
    template<typename T>
    int _get(const std::string &payload, const std::string &attribute, const std::string &key, T &t)
    {
        nlohmann::json json;
        
        try {
            json = nlohmann::json::parse(payload);
        }
        catch (nlohmann::json::parse_error &e) {
            return -1;
        }
        try {
            t = json[attribute][key].get<T>();
        }
        catch (nlohmann::json::type_error &e) {
            return -1;
        }
        return 0;
    }

    int get_string(const std::string &payload, const std::string &attribute, const std::string &key, std::string &str);
    int get_int(const std::string &payload, const std::string &attribute, const std::string &key, int &i);
    int get_unsigned_int(const std::string &payload, const std::string &attribute, const std::string &key, unsigned &u);
    int get_double(const std::string &payload, const std::string &attribute, const std::string &key, double &f);
    int get_bool(const std::string &payload, const std::string &attribute, const std::string &key, bool &b);
};
