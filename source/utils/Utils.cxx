#include <utils/Utils.hxx>

#include <fstream>
namespace Utils
{
    namespace Json
    {
        int ParseJson(const std::string &payload, json &data)
        {
            try {
                data = json::parse(payload);
            }
            catch (json::parse_error &error) {
                spdlog::error("Could not parse JSON : {:s}", error.what());
                return -1;
            }
            return 0;
        }

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

        int ToDouble(const json &data, const std::string &key, double &value)
        {
            if (KeyExists(data, key) == false)
                return -1;
            if (data[key].is_number_float() == false) {
                spdlog::error("{:s} must be a float", key);
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
        bool StartsWith(const std::string &s, const std::string &prefix)
        {
            return s.rfind(prefix, 0) == 0;
        }
    }
};