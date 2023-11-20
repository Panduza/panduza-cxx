#pragma once

#include <condition_variable>
#include <map>
#include <string>

#include <mqtt/client.h>
#include <nlohmann/json.hpp>

#include "field.hxx"
#include "core.hxx"

class attribute
{
public:
    explicit attribute(const std::string &name);

    template <typename T>
    void add_ro_field(const std::string &name)
    {
        _add_field<T>(name, access_mode::readonly);
    }

    template <typename T>
    void add_rw_field(const std::string &name)
    {
        _add_field<T>(name, access_mode::readwrite);
    }

    void on_message(const mqtt::const_message_ptr &message);

    static bool type_is_compatible(const nlohmann::json::value_t &value1, const nlohmann::json::value_t &value2);

    template <typename T>
    field<T> &get_field(const std::string &name)
    {
        return std::any_cast<field<T> &>(_fields[name]);
    }

    const std::string &get_name() const { return _name; }

    void set_callback(std::function<void(const nlohmann::json &data)> cb) { _callback = cb; }

private:
    static constexpr int SET_TIMEOUT = 1; // in seconds
    static constexpr int SET_TIMEOUT_RETRIES = 3;

    int _data_from_field(const nlohmann::json &data);

    template <typename T>
    void _add_field(const std::string &name, access_mode mode)
    {
        field<T> field(name, mode);

        field.set_callback(std::bind(&attribute::_data_from_field, this, std::placeholders::_1));
        _fields[name] = field;
    }

    template <typename T>
    void _assign_value(std::any &elem, const nlohmann::json &data)
    {
        try
        {
            auto &f = std::any_cast<field<T> &>(elem);
            if (type_is_compatible(data.type(), f.get_json_type()) == true)
                f.set_value(data.get<T>());
            else
                spdlog::error("Type mismatch for attribute {:s}, field {:s}.. ", _name, f.name());
        }
        catch (const std::bad_any_cast &e)
        {
            spdlog::error("Bad any cast for attribute {:s} : {}", _name, e.what());
        }
    }

    std::unordered_map<std::string, std::any> _fields;
    std::string _name;
    std::condition_variable _cv;
    std::mutex _mtx;
    bool _waiting_for_response = false;
    std::function<void(const nlohmann::json &data)> _callback = nullptr;
    bool _is_init = false;
};
