#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <variant>

#include <mqtt/message.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "../utils/json_attribute.hxx"

class attribute
{
public:
    using s_ptr = std::shared_ptr<attribute>;

    explicit attribute(const std::string &name);
    ~attribute();

    const std::string &get_name() const { return _name; }
    
    template<typename T>
    void register_field(const std::string &name)
    {
        _fields[name] = T();
    }

    template<typename T>
    const T &get_field(const std::string &name) 
    {
        try {
            return std::get<T>(_fields[name]);
        }
        catch (const std::bad_variant_access &e) {
            spdlog::error("attribute::get_field: {}", e.what());
            throw;
        }
    }

    template<typename T>
    int set_field(const std::string &field, const T &val)
    {
        nlohmann::json data;
        std::mutex mtx;
        std::unique_lock<std::mutex> lock(mtx);

        data[_name][field] = val;

        if (_msg_cb(data) < 0) {
            spdlog::error("attribute::set: failed to send message");
            return -1;
        }

        if (_cv.wait_for(lock, std::chrono::seconds(3), [&]() {
            return std::get<T>(_fields[field]) == val; }) == false)
        {
            spdlog::error("attribute::set: timed out waiting for value to be set");
            return -1;
        }
        return 0;
    }

    void on_message(mqtt::const_message_ptr msg);

    void register_callback(const std::function<void(void)> &cb);
    void remove_callback(const std::function<void(void)> &cb);

    void set_msg_callback(const std::function<int(const nlohmann::json &data)> &cb) { _msg_cb = cb; }

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
    std::list<std::function<void(void)>> _callbacks;
    std::function<int(const nlohmann::json &data)> _msg_cb;
    std::condition_variable _cv;
};