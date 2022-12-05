#include "json.hxx"

using namespace pza;
using namespace json;

int json::_parse(const std::string &payload, nlohmann::json &json)
{
    try {
        json = nlohmann::json::parse(payload);
    } catch (nlohmann::json::parse_error &e) {
        return -1;
    }
    return 0;
}

int json::get_string(const std::string &payload, const std::string &atts, const std::string &key, std::string &str)
{
    nlohmann::json json;
    if (_parse(payload, json) < 0) {
        return -1;
    }
    try {
        str = json[atts][key].get<std::string>();
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}

int json::get_int(const std::string &payload, const std::string &atts, const std::string &key, int &i)
{
    nlohmann::json json;
    if (_parse(payload, json) < 0) {
        return -1;
    }
    try {
        i = json[atts][key].get<int>();
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}

int json::get_unsigned_int(const std::string &payload, const std::string &atts, const std::string &key, unsigned &u)
{
    nlohmann::json json;
    if (_parse(payload, json) < 0) {
        return -1;
    }
    try {
        u = json[atts][key].get<unsigned>();
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}

int json::get_double(const std::string &payload, const std::string &atts, const std::string &key, double &f)
{
    nlohmann::json json;
    if (_parse(payload, json) < 0) {
        return -1;
    }
    try {
        f = json[atts][key].get<double>();
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}

int json::get_bool(const std::string &payload, const std::string &atts, const std::string &key, bool &b)
{
    nlohmann::json json;
    if (_parse(payload, json) < 0) {
        return -1;
    }
    try {
        b = json[atts][key].get<bool>();
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}

int json::get_array(const std::string &payload, const std::string &atts, const std::string &key, nlohmann::json &json)
{
    nlohmann::json j;
    if (_parse(payload, j) < 0) {
        return -1;
    }
    try {
        json = j[atts][key];
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}

int json::get_object(const std::string &payload, const std::string &atts, const std::string &key, nlohmann::json &json)
{
    nlohmann::json j;
    if (_parse(payload, j) < 0) {
        return -1;
    }
    try {
        json = j[atts][key];
    } catch (nlohmann::json::type_error &e) {
        return -1;
    }
    return 0;
}