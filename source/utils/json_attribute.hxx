#pragma once

#include <nlohmann/json.hpp>

class json_attribute
{
public:
    json_attribute(const std::string &attribute);

    int parse(const std::string &payload);

    template<typename T>
    int get(const std::string &key, T &t)
    {
        try {
            t = _json[key].get<T>();
        }
        catch (nlohmann::json::type_error &e) {
            return -1;
        }
        return 0;
    }

    int get_string(const std::string &key, std::string &str);
    int get_int(const std::string &key, int &i);
    int get_unsigned_int(const std::string &key, unsigned &u);
    int get_double(const std::string &key, double &f);
    int get_bool(const std::string &key, bool &b);

private:
    std::string _attribute;
    nlohmann::json _json;
};
