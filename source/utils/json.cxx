#include "json.hxx"

using namespace json;

int json::get_string(const std::string &payload, const std::string &atts, const std::string &key, std::string &str)
{
    return _get<std::string>(payload, atts, key, str);
}

int json::get_int(const std::string &payload, const std::string &atts, const std::string &key, int &i)
{
    return _get<int>(payload, atts, key, i);
}

int json::get_unsigned_int(const std::string &payload, const std::string &atts, const std::string &key, unsigned &u)
{
    return _get<unsigned>(payload, atts, key, u);
}

int json::get_double(const std::string &payload, const std::string &atts, const std::string &key, double &f)
{
    return _get<double>(payload, atts, key, f);
}

int json::get_bool(const std::string &payload, const std::string &atts, const std::string &key, bool &b)
{
    return _get<bool>(payload, atts, key, b);
}
