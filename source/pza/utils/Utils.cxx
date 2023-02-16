#include <pza/utils/Utils.hxx>

namespace pza
{
    namespace utils
    {
        namespace json
        {
            int ParseJson(const std::string &payload, nlohmann::json &data)
            {
                try
                {
                    data = nlohmann::json::parse(payload);
                }
                catch (nlohmann::json::parse_error &error)
                {
                    spdlog::error("Could not parse JSON : {:s}", error.what());
                    return -1;
                }
                return 0;
            }

            bool KeyExists(const nlohmann::json &data, const std::string &key)
            {
                if (data.contains(key) == false)
                {
                    spdlog::error("Json object does not contain {:s}", key);
                    return false;
                }
                return true;
            }

            int ToString(const nlohmann::json &data, const std::string &key, std::string &value)
            {
                if (KeyExists(data, key) == false)
                    return -1;
                if (data[key].is_string() == false)
                {
                    spdlog::error("{:s} must be a string", key);
                    return -1;
                }
                value = data[key];
                return 0;
            }

            int ToDouble(const nlohmann::json &data, const std::string &key, double &value)
            {
                if (KeyExists(data, key) == false)
                    return -1;
                if (data[key].is_number_float() == false)
                {
                    spdlog::error("{:s} must be a float", key);
                    return -1;
                }
                value = data[key];
                return 0;
            }

            int ToInteger(const nlohmann::json &data, const std::string &key, int &value)
            {
                if (KeyExists(data, key) == false)
                    return -1;
                if (data[key].is_number_integer() == false)
                {
                    spdlog::error("{:s} must be an integer", key);
                    return -1;
                }
                value = data[key];
                return 0;
            }

            int ToBool(const nlohmann::json &data, const std::string &key, bool &value)
            {
                if (KeyExists(data, key) == false)
                    return -1;
                if (data[key].is_boolean() == false)
                {
                    spdlog::error("{:s} must be a boolean", key);
                    return -1;
                }
                value = data[key];
                return 0;
            }

            int ToObject(const nlohmann::json &data, const std::string &key, nlohmann::json &value)
            {
                if (KeyExists(data, key) == false)
                    return -1;
                if (data[key].is_object() == false)
                {
                    spdlog::error("{:s} must be an object", key);
                    return -1;
                }
                value = data[key];
                return 0;
            }

            std::string GetTypeName(const nlohmann::json::value_t &type)
            {
                switch (type)
                {
                case nlohmann::json::value_t::null:
                    return "null";
                case nlohmann::json::value_t::boolean:
                    return "boolean";
                case nlohmann::json::value_t::number_integer:
                    return "integer";
                case nlohmann::json::value_t::number_unsigned:
                    return "unsigned";
                case nlohmann::json::value_t::number_float:
                    return "float";
                case nlohmann::json::value_t::string:
                    return "string";
                case nlohmann::json::value_t::array:
                    return "array";
                case nlohmann::json::value_t::object:
                    return "object";
                default:
                    return "unknown";
                }
            }
        }
        namespace string
        {
            bool StartsWith(const std::string &s, const std::string &prefix)
            {
                return s.rfind(prefix, 0) == 0;
            }
        }
    };
};