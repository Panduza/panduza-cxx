#include <utils/Utils.hxx>

namespace Utils
{
    namespace Json
    {
        bool KeyExists(const json &data, const std::string &key)
        {
            if (data.contains(key) == false) {
                spdlog::error("Json object does not contain {:s}", key);
                return false;
            }
            return true;
        }
        
        int ToString(const json &data, const std::string &key, std::string &value)
        {
            if (KeyExists(data, key) == false)
                return -1;
            if (data[key].is_string() == false) {
                spdlog::error("{:s} must be a string", key);
                return -1;
            }
            value = data[key];
            return 0;
        }
        
        int ToInteger(const json &data, const std::string &key, int &value)
        {
            if (KeyExists(data, key) == false)
                return -1;
            if (data[key].is_number_integer() == false) {
                spdlog::error("{:s} must be an integer", key);
                return -1;
            }
            value = data[key];
            return 0;
        }
        
        int ToBool(const json &data, const std::string &key, bool &value)
        {
            if (KeyExists(data, key) == false)
                return -1;
            if (data[key].is_boolean() == false) {
                spdlog::error("{:s} must be a boolean", key);
                return -1;
            }
            value = data[key];
            return 0;
        }
        
        int ToObject(const json &data, const std::string &key, json &value)
        {
            if (KeyExists(data, key) == false)
                return -1;
            if (data[key].is_object() == false) {
                spdlog::error("{:s} must be an object", key);
                return -1;
            }
            value = data[key];
            return 0;
        }
    }
    namespace String
    {
        bool EndsWith(std::string_view str, std::string_view suffix)
        {
            return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
        }

        bool StartsWith(std::string_view str, std::string_view prefix)
        {
            return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
        }
    }
};