#pragma once

#include <nlohmann/json.hpp>

namespace pza
{
    namespace json
    {
        int get_string(const std::string &payload, const std::string &atts, const std::string &key, std::string &str);
        int get_int(const std::string &payload, const std::string &atts, const std::string &key, int &i);
        int get_unsigned_int(const std::string &payload, const std::string &atts, const std::string &key, unsigned &u);
        int get_double(const std::string &payload, const std::string &atts, const std::string &key, double &f);
        int get_bool(const std::string &payload, const std::string &atts, const std::string &key, bool &b);
        int get_array(const std::string &payload, const std::string &atts, const std::string &key, nlohmann::json &json);
        int get_object(const std::string &payload, const std::string &atts, const std::string &key, nlohmann::json &json);
        int _parse(const std::string &payload, nlohmann::json &json);
    };
};
