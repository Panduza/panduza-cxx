#include "client.hxx"

client::client(const std::string &addr, int port, std::optional<std::string> id)
    : _addr(addr),
    _port(port),
    _scanner(this)
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

int client::connect(void)
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

int client::disconnect(void)
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

void client::connection_lost(const std::string &cause)
{
    spdlog::error("connection lost: {}", cause);
}

int client::publish(const std::string &topic, const std::string &payload)
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

int client::subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb)
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

int client::unsubscribe(const std::string &topic)
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

void client::message_arrived(mqtt::const_message_ptr msg)
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

device::ptr client::register_device(const std::string &group, const std::string &name)
{
    device::ptr dev;

    try {
        dev = std::make_shared<device>(this, group, name);
    }
    catch (const std::exception &exc) {
        spdlog::error("failed to register device {}/{}: {}", group, name, exc.what());
        return nullptr;
    }
    _devices.emplace(group + "/" + name, dev);
    return dev;
}

void client::register_all_devices()
{
    for (auto &it : _scanner.get_devices()) {
        topic t(it.first);
        
        if (t.is_valid() == true) {
            if (register_device(t.get_group(), t.get_device_name()) < 0) {
                continue;
            }
        }
    }
}
