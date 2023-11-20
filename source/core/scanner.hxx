#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <set>

#include <spdlog/spdlog.h>
#include <mqtt/client.h>

#include "../utils/json.hxx"

class scanner
{
public:
    using device_map = std::unordered_map<std::string, std::string>;
    using interface_map = std::unordered_map<std::string, std::string>;

    struct client_callbacks {
        std::function<int(const std::string &, const std::string &)> publish;
        std::function<int(const std::string &, const std::function<void(mqtt::const_message_ptr)> &)> subscribe;
        std::function<int(const std::string &)> unsubscribe;
    };

    scanner(const struct client_callbacks &callbacks);

    void set_scan_timeout(int timeout) { _scan_timeout = timeout; }
    int get_scan_timeout(void) const { return _scan_timeout; }

    int scan();
    int scan_interfaces(const std::string &group, const std::string &name);
    int scan_device_identity(const std::string &group, const std::string &name);

    bool device_was_scanned(const std::string &group, const std::string &name);

    const device_map &get_devices(void) const { return _devices; }
    const interface_map &get_interfaces(void) const { return _interfaces; }
    std::string get_device_identity() const { return _device_identity; }

private:
    static constexpr int _scan_timeout_default = 3; // in seconds

    struct client_callbacks _callbacks;
    std::mutex _mtx;
    std::condition_variable _cv;
    int _scan_timeout = _scan_timeout_default;

    std::set<std::string> _platforms;
    unsigned int _device_count;
    device_map _devices;
    unsigned int _interface_count;
    interface_map _interfaces;
    std::string _device_identity;

    int _scan_platforms();
    int _scan_devices();

    void _on_platform_info(mqtt::const_message_ptr);
    void _on_device_info(mqtt::const_message_ptr);
    void _on_interface_info(mqtt::const_message_ptr);
    void _on_identity_info(mqtt::const_message_ptr);
};