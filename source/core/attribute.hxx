#pragma once

#include <string>
#include <unordered_map>
#include <variant>

#include <mqtt/message.h>
#include <spdlog/spdlog.h>

#include "../utils/json_attribute.hxx"

class attribute
{
public:
    explicit attribute(const std::string &name);

    template<typename T>
    void register_field(const std::string &name)
    {
        _fields[name] = T();
    }

    template<typename T>
    const T &get_field(const std::string &name)
    {
        return std::get<T>(_fields[name]);
    }

    void on_message(mqtt::const_message_ptr msg);
    const std::string &get_name() const { return _name; }

private:
    using field_types = std::variant<std::string, unsigned int, int, double, bool>;

    template<typename T>
    void _set_field(json_attribute &json, const std::string &name)
    {
        T val;
        if (json.get<T>(name, val) < 0) {
            spdlog::error("attribute::on_message: failed to get field {}", name);
            return ;
        }
        std::get<T>(_fields[name]) = val;
    }

    std::string _name;
    std::unordered_map<std::string, field_types> _fields;
};