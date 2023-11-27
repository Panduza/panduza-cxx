#include <set>
#include <random>

#include <pza/core/client.hxx>
#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include <mqtt/client.h>
#include <spdlog/spdlog.h>

#include "../utils/json.hxx"
#include "../utils/topic.hxx"
#include "scanner.hxx"
#include "interface_factory.hxx"
#include "attribute.hxx"
#include "mqtt_service.hxx"

using namespace pza;

static constexpr int conn_timeout_default = 5; // in seconds

struct client_impl : mqtt_service
{
    explicit client_impl(const std::string &addr, int port, std::optional<std::string> id = std::nullopt);

    int connect(void);
    int disconnect(void);
    bool is_connected(void) const { return (_paho_client->is_connected()); }

    const std::string &get_addr(void) const { return _addr; }
    const std::string &get_id(void) const { return _id; }
    int get_port(void) const { return _port; }

    void set_conn_timeout(int timeout) { _conn_timeout = timeout; }
    int get_conn_timeout(void) const { return _conn_timeout; }

    int publish(const std::string &topic, const std::string &payload) override;
    int publish(const mqtt::message::const_ptr_t msg) override;
    int subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb) override;
    int unsubscribe(const std::string &topic) override;

    void connection_lost(const std::string &cause);
    void message_arrived(mqtt::const_message_ptr msg);

    device::s_ptr register_device(const std::string &group, const std::string &name);

    int scan_platforms();
    int scan_devices();

    void on_platform_info(mqtt::const_message_ptr msg);
    void on_device_info(mqtt::const_message_ptr msg);

    mqtt::async_client::ptr_t _paho_client;
    unsigned int _conn_timeout = conn_timeout_default;
    std::string _addr;
    int _port;
    std::string _id;
    std::mutex _mtx;
    std::unordered_map<std::string, std::function<void(mqtt::const_message_ptr)>> _listeners;
    std::unordered_map<std::string, device::s_ptr> _devices;
    std::set<std::string> _platforms_scanned;
    std::unordered_map<std::string, std::string> _devices_scanned;
    unsigned int _device_count = 0;
};

client_impl::client_impl(const std::string &addr, int port, std::optional<std::string> id)
    : _addr(addr),
    _port(port)
{
    std::string url = "tcp://" + addr + ":" + std::to_string(port);

    if (id.has_value() == false) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1000000);
        _id = "pza_" + std::to_string(dis(gen));
        spdlog::warn("no client id provided, using generated id: {}", _id);
    }
    else
        _id = id.value();
    _paho_client = std::make_unique<mqtt::async_client>(url, _id);
    _paho_client->set_message_callback(std::bind(&client_impl::message_arrived, this, std::placeholders::_1));
    _paho_client->set_connection_lost_handler(std::bind(&client_impl::connection_lost, this, std::placeholders::_1));
    spdlog::trace("created client with id: {}", _id);
}

int client_impl::connect(void)
{
    mqtt::connect_options connOpts;

    spdlog::debug("Attempting connection to {}...", _addr);

    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    try {
        _paho_client->connect(connOpts)->wait_for(std::chrono::seconds(_conn_timeout));
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to connect to client: {}", exc.what());
        return -1;
    }

    spdlog::info("connected to {}", _addr);
    return 0;
}

int client_impl::disconnect(void)
{
    spdlog::debug("Attempting to disconnect from {}...", _addr);

    try {
        _paho_client->disconnect()->wait_for(std::chrono::seconds(_conn_timeout));
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to disconnect: {}", exc.what());
        return -1;
    }
    spdlog::info("disconnected from {}", _addr);
    return 0;
}

void client_impl::connection_lost(const std::string &cause)
{
    spdlog::error("connection lost: {}", cause);
}

int client_impl::publish(const std::string &topic, const std::string &payload)
{
    return publish(mqtt::make_message(topic, payload));
}

int client_impl::publish(const mqtt::const_message_ptr msg)
{
    try {
        _paho_client->publish(msg)->wait_for(std::chrono::seconds(_conn_timeout));
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to publish: {}", exc.what());
        return -1;
    }
    spdlog::trace("published message {} to {}", msg->get_payload_str(), msg->get_topic());
    return 0;
}

int client_impl::subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb)
{
    std::string t;

    t = topic::regexify_topic(topic);
    _listeners.emplace(t, cb);
    try {
        _paho_client->subscribe(topic, 0)->wait_for(std::chrono::seconds(_conn_timeout));
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to subscribe: {}", exc.what());
        _listeners.erase(t);
        return -1;
    }
    spdlog::trace("subscribed to topic: {}", topic);
    return 0;
}

int client_impl::unsubscribe(const std::string &topic)
{
    std::string t;

    try {
        _paho_client->unsubscribe(topic)->wait_for(std::chrono::seconds(_conn_timeout));
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to unsubscribe: {}", exc.what());
        return -1;
    }
    t = topic::regexify_topic(topic);
    for (auto it = _listeners.begin(); it != _listeners.end(); ) {
        if (topic::topic_matches(it->first, t)) {
            it = _listeners.erase(it);
        }
        else
            ++it;
    }
    spdlog::trace("unsubscribed from topic: {}", topic);
    return 0;
}

void client_impl::message_arrived(mqtt::const_message_ptr msg)
{
    spdlog::trace("message arrived on topic: {}", msg->get_topic());

    if (_listeners.count(msg->get_topic()) > 0) {
        _listeners[msg->get_topic()](msg);
        return;
    }

    for (auto &it : _listeners) {
        if (topic::topic_matches(msg->get_topic(), it.first)) {
            it.second(msg);
        }
    }
}

int client_impl::scan_platforms()
{
    scanner scanner(*this);

    _platforms_scanned.clear();

    scanner.set_scan_timeout(5)
        .set_message_callback(std::bind(&client_impl::on_platform_info, this, std::placeholders::_1))
        .set_condition_callback([&](void) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return true;
        })
        .set_publisher(mqtt::make_message("pza", "p"))
        .set_subscription_topic("pza/server/+/+/atts/info");

    if (scanner.run() == false) {
        spdlog::error("timed out waiting for platforms");
        return -1;
    }

    if (_platforms_scanned.empty()) {
        spdlog::error("No platforms found");
        return -1;
    }

    if (_device_count == 0) {
        spdlog::error("No devices found on scanned platforms");
        return -1;
    }
    spdlog::info("Found {} platform{} with {} devices", _platforms_scanned.size(), _platforms_scanned.size() > 1 ? "s" : "", _device_count);
    return 0;
}

void client_impl::on_platform_info(mqtt::const_message_ptr msg)
{
    const std::string &payload = msg->get_payload_str();
    const std::string &topic = msg->get_topic();
    std::string type;
    unsigned int val;

    if (json::get_string(payload, "info", "type", type) < 0) {
        spdlog::error("failed to parse type info: {}", payload);
        return;
    }
    if (type != "platform" || _platforms_scanned.find(topic) != _platforms_scanned.end())
        return;

    _platforms_scanned.insert(topic);

    spdlog::trace("received platform info: {}", payload);

    if (json::get_unsigned_int(payload, "info", "number_of_devices", val) < 0) {
        spdlog::error("failed to parse platform info: {}", payload);
        return;
    }
    _device_count += val;
}

int client_impl::scan_devices()
{
    scanner scanner(*this);

    _devices_scanned.clear();

    scanner.set_message_callback(std::bind(&client_impl::on_device_info, this, std::placeholders::_1))
        .set_condition_callback([&](void) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return true;
        })
        .set_publisher(mqtt::make_message("pza", "d"))
        .set_subscription_topic("pza/+/+/device/atts/info");

    if (scanner.run() == false) {
        spdlog::error("timed out waiting for devices");
        return -1;
    }
    return 0;
}

void client_impl::on_device_info(mqtt::const_message_ptr msg)
{
    std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/device/atts/info"));

    spdlog::trace("received device info: {} {}", msg->get_topic(), msg->get_payload_str());
    _devices_scanned.emplace(base_topic, msg->get_payload_str());
}

device::s_ptr client_impl::register_device(const std::string &group, const std::string &name)
{
    device_info info;
    device::s_ptr dev;

    info.group = group;
    info.name = name;
    /*

    if (_scanner.device_was_scanned(group, name) == false) {
        spdlog::error("device {} was not scanned", name);
        return nullptr;
    }

    if (_scanner.scan_device_identity(group, name) < 0) {
        spdlog::error("failed to scan identity for device {}", name);
        return nullptr;
    }

    if (json::get_string(_scanner.get_device_identity(), "identity", "model", info.model) < 0) {
        spdlog::error("Device {} does not have a model", name);
        return nullptr;
    }

    if (json::get_string(_scanner.get_device_identity(), "identity", "manufacturer", info.manufacturer) < 0) {
        spdlog::error("Device {} does not have a manufacturer", name);
        return nullptr;
    }

    if (json::get_string(_scanner.get_device_identity(), "identity", "family", info.family) < 0) {
        spdlog::error("Device {} does not have a family", name);
        return nullptr;
    }

    if (_scanner.scan_interfaces(group, name) < 0) {
        spdlog::error("failed to scan interfaces for device {}", name);
        return nullptr;
    }

    std::transform(info.family.begin(), info.family.end(), info.family.begin(), ::tolower);
*/
    return std::make_shared<device>(*this, info);
}

client::client(const std::string &addr, int port, std::optional<std::string> id)
    : _impl(std::make_unique<client_impl>(addr, port, id))
{

}

client::~client()
{

}

int client::connect(void)
{
    return _impl->connect();
}

int client::disconnect(void)
{
    return _impl->disconnect();
}

bool client::is_connected(void) const
{
    return _impl->is_connected();
}

const std::string &client::get_addr(void) const
{
    return _impl->get_addr();
}

const std::string &client::get_id(void) const
{
    return _impl->get_id();
}

int client::get_port(void) const
{
    return _impl->get_port();
}

void client::set_conn_timeout(int timeout)
{
    _impl->set_conn_timeout(timeout);
}

int client::get_conn_timeout(void) const
{
    return _impl->get_conn_timeout();
}

device::s_ptr client::register_device(const std::string &group, const std::string &name)
{
    return _impl->register_device(group, name);
}

int client::scan()
{
    if (_impl->scan_platforms() < 0)
        return -1;
    return _impl->scan_devices();
}
