#include <random>

#include <pza/core/client.hxx>
#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include <mqtt/client.h>
#include <spdlog/spdlog.h>

#include "../utils/topic.hxx"
#include "scanner.hxx"
#include "interface_factory.hxx"
#include "attribute.hxx"

using namespace pza;

class client_priv : public mqtt::callback
{
public:
    explicit client_priv(const std::string &addr, int port, std::optional<std::string> id = std::nullopt);

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

    scanner &get_scanner(void) { return _scanner; }
    void on_new_device(device *dev);
    void on_new_attributes(const std::string &itf_topic, const std::vector<attribute *> &attributes);

    void add_device(device::s_ptr &dev) { _devices[dev->get_name()] = dev; }
    
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
    std::unordered_map<std::string, device::s_ptr> _devices;

    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

    int _register_interface(itf::s_ptr &interface);
};

client_priv::client_priv(const std::string &addr, int port, std::optional<std::string> id)
    : _addr(addr),
    _port(port),
    _scanner(
        scanner::client_callbacks{
            std::bind(&client_priv::publish, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&client_priv::subscribe, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&client_priv::unsubscribe, this, std::placeholders::_1)
        }
    )
{
    std::string url = "tcp://" + addr + ":" + std::to_string(port);

    if (id.has_value() == false) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1000000);
        _id = "pza_" + std::to_string(dis(gen));
    }
    else
        _id = id.value();
    _paho_client = std::make_unique<mqtt::async_client>(url, _id);
    _paho_client->set_callback(*this);
    spdlog::trace("created client with id: {}", _id);
}

int client_priv::connect(void)
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
    if (_scanner.scan() < 0) {
        disconnect();
        spdlog::error("failed to connect to {}", _addr);
        return -1;
    }
    spdlog::info("connected to {}", _addr);
    return 0;
}

int client_priv::disconnect(void)
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

void client_priv::connection_lost(const std::string &cause)
{
    spdlog::error("connection lost: {}", cause);
}

int client_priv::publish(const std::string &topic, const std::string &payload)
{
    mqtt::message_ptr pubmsg;

    pubmsg = mqtt::make_message(topic, payload);

    try {
        _paho_client->publish(pubmsg)->wait();
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to publish: {}", exc.what());
        return -1;
    }
    spdlog::trace("published message {} to {}", payload, topic);
    return 0;
}

int client_priv::subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb)
{
    std::string t;

    t = topic::regexify_topic(topic);
    _listeners.emplace(t, cb);
    try {
        _paho_client->subscribe(topic, 0)->wait();
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to subscribe: {}", exc.what());
        _listeners.erase(t);
        return -1;
    }
    spdlog::trace("subscribed to topic: {}", topic);
    return 0;
}

int client_priv::unsubscribe(const std::string &topic)
{
    std::string t;

    try {
        _paho_client->unsubscribe(topic)->wait();
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

void client_priv::message_arrived(mqtt::const_message_ptr msg)
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

void client_priv::on_new_device(device *dev)
{
    for (const auto &elem : _scanner.get_interfaces()) {
        std::string type;

        if (json::get_string(elem.second, "info", "type", type) < 0) {
            spdlog::error("interface {} does not have a type", elem.first);
            continue;
        }
        itf::s_ptr new_itf = interface_factory::create_interface(dev, elem.first, type, itf::client_callbacks{
            std::bind(&client_priv::on_new_attributes, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&client_priv::publish, this, std::placeholders::_1, std::placeholders::_2)
        });
        if (new_itf == nullptr)
            continue;
        dev->add_interface(new_itf);
    }
}

void client_priv::on_new_attributes(const std::string &itf_topic, const std::vector<attribute *> &attributes) {
    size_t atts_processed = 0;
    const size_t atts_count = attributes.size();
    std::condition_variable cv;
    std::mutex cv_m;
    bool ret;

    for (auto &it : attributes) {
        std::string topic = itf_topic + "/atts/" + it->get_name();
        subscribe(topic, [&](mqtt::const_message_ptr msg) {
            it->on_message(msg);
            atts_processed++;
            cv.notify_one();
        });
    }
    std::unique_lock<std::mutex> lock(cv_m);
    ret = cv.wait_for(lock, std::chrono::seconds(_scanner.get_scan_timeout()), [&atts_count, &atts_processed] {
        return atts_processed == atts_count;
    });

    for (auto &it : attributes) {
        std::string topic = itf_topic + "/atts/" + it->get_name();
        unsubscribe(topic);
        if (ret)
            subscribe(topic, std::bind(&attribute::on_message, it, std::placeholders::_1));
    }
}
client::client(const std::string &addr, int port, std::optional<std::string> id)
    : _priv(std::make_unique<client_priv>(addr, port, id))
{

}

client::~client()
{

}

int client::connect(void)
{
    return _priv->connect();
}

int client::disconnect(void)
{
    return _priv->disconnect();
}

bool client::is_connected(void) const
{
    return _priv->is_connected();
}

const std::string &client::get_addr(void) const
{
    return _priv->get_addr();
}

const std::string &client::get_id(void) const
{
    return _priv->get_id();
}

int client::get_port(void) const
{
    return _priv->get_port();
}

void client::set_conn_timeout(int timeout)
{
    _priv->set_conn_timeout(timeout);
}

int client::get_conn_timeout(void) const
{
    return _priv->get_conn_timeout();
}

device::s_ptr client::register_device(const std::string &group, const std::string &name)
{
    itf::s_ptr itf;
    device_info info;
    device::s_ptr dev;
    scanner &_scanner = _priv->get_scanner();

    info.group = group;
    info.name = name;

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
    
    dev = std::make_shared<device>(this, info, std::bind(&client_priv::on_new_device, _priv.get(), std::placeholders::_1));
    _priv->add_device(dev);
    return dev;
}