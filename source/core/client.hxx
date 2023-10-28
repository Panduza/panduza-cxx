#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <regex>

#include <magic_enum.hpp>
#include <mqtt/client.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "../utils/topic.hxx"
#include "device.hxx"
#include "scanner.hxx"

class client : public mqtt::callback
{
public:
    using ptr = std::shared_ptr<client>;

    explicit client(const std::string &addr, int port, std::optional<std::string> id = std::nullopt);

    int connect(void);
    int disconnect(void);
    bool is_connected(void) const { return (_paho_client->is_connected()); }

    const std::string &get_addr(void) const { return _addr; }
    const std::string &get_id(void) const { return _id; }
    int get_port(void) const { return _port; }

    void set_conn_timeout(int timeout) { _conn_timeout = timeout; }
    int get_conn_timeout(void) const { return _conn_timeout; }

    int publish(const std::string &topic, const std::string &payload);
    int subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb);
    int unsubscribe(const std::string &topic);

    device::ptr register_device(const std::string &group, const std::string &name);
    void register_all_devices();

    scanner &get_scanner(void) { return _scanner; }

private:
    static constexpr int conn_timeout_default = 5; // in seconds

    mqtt::async_client::ptr_t _paho_client;
    unsigned int _conn_timeout = conn_timeout_default;
    std::string _addr;
    int _port;
    std::string _id;
    std::mutex _mtx;
    std::unordered_map<std::string, std::function<void(mqtt::const_message_ptr)>> _listeners;
    scanner _scanner;
    std::unordered_map<std::string, device::ptr> _devices;

    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;
};
