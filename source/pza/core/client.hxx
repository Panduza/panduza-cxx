#pragma once

#include <memory>
#include <random>
#include <condition_variable>
#include <mutex>
#include <regex>

#include <mqtt/client.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <pza/core/core.hxx>
#include <pza/core/device_factory.hxx>
#include <pza/utils/json.hxx>
#include <pza/utils/topic.hxx>

namespace pza
{
    class client : virtual public mqtt::callback
    {
    public:
        using ptr = std::shared_ptr<client>;

        friend class device;
        friend class itface;

        explicit client(const std::string &addr, int port, const std::string &id = "");

        int connect(void);
        int disconnect(void);
        int scan(void);
        bool is_connected(void) const { return (_paho_client->is_connected()); }

        void set_scan_timeout(unsigned int timeout) { _scan_timeout = timeout; }
        unsigned int get_scan_timeout(void) const { return _scan_timeout; }

        const std::string &get_addr(void) const { return _addr; }
        const std::string &get_id(void) const { return _id; }
        int get_port(void) const { return _port; }

        int register_device(const device::ptr &device);
        int register_all_devices();

        device::ptr find_device(const std::string &group, const std::string &name);

        using device_map = std::map<std::string, device::ptr>;

        const device_map &get_devices(void) const { return _devices; }

    private:
        using listener_map = std::map<std::string, std::function<void(mqtt::const_message_ptr)>>;

        static constexpr unsigned int SCAN_TIMEOUT_DEFAULT = 5;

        unsigned int _scan_timeout = SCAN_TIMEOUT_DEFAULT;
        std::string _addr;
        int _port;
        std::string _id;
        mqtt::async_client::ptr_t _paho_client;
        std::mutex _mtx;
        listener_map _listeners;

        std::map<std::string, std::string> _scan_device_results;
        unsigned int _scan_device_count_expected = 0;
        
        std::map<std::string, std::string> _scan_itf_results;
        unsigned int _scan_itf_count_expected = 0;
        
        std::map<std::string, device::ptr> _devices;

        void connection_lost(const std::string &cause) override;
        void message_arrived(mqtt::const_message_ptr msg) override;

        int _publish(const std::string &topic, const std::string &payload);
        int _subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb);
        int _unsubscribe(const std::string &topic);

        std::string _regexify_topic(const std::string &topic);
        std::string _convertPattern(const std::string &fnmatchPattern);
        bool _topic_matches(const std::string &str, const std::string &fnmatchPattern);
        void _count_devices_to_scan(const std::string &payload);
        int _scan_platforms();
        int _scan_devices();
        int _scan_interfaces(std::unique_lock<std::mutex> &lock, const device::ptr &device);
        device::ptr create_device(const std::string &topic);
    };
};
