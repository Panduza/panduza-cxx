#include "client.hxx"

using namespace pza;

static constexpr unsigned int CONN_TIMEOUT = 5; // in seconds

client::client(const std::string &addr, int port, const std::string &id)
    : _addr(addr),
    _port(port),
    _id(id)
{
    std::string url = "tcp://" + addr + ":" + std::to_string(port);

    if (id.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1000000);
        _id = "pza_" + std::to_string(dis(gen));
    }
    else
        _id = id;
    _paho_client = std::make_unique<mqtt::async_client>(url, id);
    _paho_client->set_callback(*this);
    spdlog::trace("created client with id: {}", _id);
}

int client::connect(void)
{
    int ret;

    spdlog::debug("Attempting connection to {}...", _addr);

    mqtt::connect_options connOpts;

    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);
    _paho_client->set_callback(*this);

    try {
        _paho_client->connect(connOpts)->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to connect to client: {}", exc.what());
        return -1;
    }
    ret = scan();
    if (ret == 0)
        spdlog::info("connected to {}", _addr);
    else {
        disconnect();
        spdlog::error("failed to connect to {}", _addr);
    }
    return ret;
}

int client::disconnect(void)
{
    spdlog::debug("Attempting to disconnect from {}...", _addr);

    try {
        _paho_client->disconnect()->wait_for(std::chrono::seconds(CONN_TIMEOUT));
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

int client::scan()
{
    if (_scan_platforms() == -1)
        return -1;
    if (_scan_devices() == -1)
        return -1;
    return 0;
}

int client::_publish(const std::string &topic, const std::string &payload)
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

std::string client::_regexify_topic(const std::string &topic)
{
    std::string t = topic;

    std::replace(t.begin(), t.end(), '+', '*');
    std::replace(t.begin(), t.end(), '#', '*');

    return t;
}

std::string client::_convertPattern(const std::string &fnmatchPattern) {
    std::string regexPattern;
    for(auto& ch : fnmatchPattern){
        if(ch == '*') regexPattern += ".*";
        else if(ch == '/') regexPattern += "\\/";
        else regexPattern += ch;
    }
    regexPattern = "^" + regexPattern + "$"; // match the whole string
    return regexPattern;
}

bool client::_topic_matches(const std::string &str, const std::string &fnmatchPattern) {
    std::string regexPattern = _convertPattern(fnmatchPattern);
    std::regex pattern(regexPattern);
    return std::regex_match(str, pattern);
}

int client::_subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb)
{
    std::string t;

    t = _regexify_topic(topic);
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

int client::_unsubscribe(const std::string &topic)
{
    std::string t;

    try {
        _paho_client->unsubscribe(topic)->wait();
    }
    catch (const mqtt::exception &exc) {
        spdlog::error("failed to unsubscribe: {}", exc.what());
        return -1;
    }
    spdlog::trace("unsubscribed from topic: {}", topic);
    t = _regexify_topic(topic);
    for (auto it = _listeners.begin(); it != _listeners.end(); ) {
        if (_topic_matches(it->first, t)) {
            it = _listeners.erase(it);
        }
        else
            ++it;
    }
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
        if (_topic_matches(msg->get_topic(), it.first)) {
            it.second(msg);
        }
    }
}

// ============================================================================
//
int client::_scan_platforms(void)
{
    bool ret;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(_mtx);

    // Reset result variables
    _scan_device_count_expected = 0;
    _scan_device_results.clear();

    // Log
    spdlog::debug("scanning for platforms on {}...", _addr);

    // Prepare platform scan message processing
    _subscribe("pza/server/+/+/atts/info", [&](const mqtt::const_message_ptr &msg) {
        // Prepare data
        std::string payload = msg->get_payload_str();
        std::string topic = msg->get_topic();
        std::string type;
        unsigned int val;

        // Exclude other messages than platform
        if (pza::json::get_string(payload, "info", "type", type) == -1) {
            spdlog::error("failed to parse type info: {}", payload);
            return;
        }

        // ignore machinese
        if (type != "platform")
            return;

        spdlog::debug("received platform info: {}", payload);

        // @TODO HERE we should also check that we did not get the same platform twice (in the case 2 scan is performed the same time)

        // Get the number of devices
        if (pza::json::get_unsigned_int(payload, "info", "number_of_devices", val) == -1) {
            spdlog::error("failed to parse platform info: {}", payload);
            return;
        }
        _scan_device_count_expected += val;
        cv.notify_all();
    });

    // Request scan for platforms and just wait for answers
    _publish("pza", "p");
    ret = cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&](void) {
        return (_scan_device_count_expected);
    });
    _unsubscribe("pza/server/+/+/atts/info");

    if (ret == false) {
        spdlog::error("Platform scan timed out");
        return -1;
    }
    return 0;
}

// ============================================================================
// 
int client::_scan_devices(void)
{
    bool ret;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(_mtx);

    // Log
    spdlog::debug("scanning for devices on {}...", _addr);

    // Prepare device scan message processing
    _subscribe("pza/+/+/device/atts/info", [&](const mqtt::const_message_ptr &msg) {
        std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/device/atts/info"));
        spdlog::debug("received device info: {} {}", msg->get_topic(), msg->get_payload_str());
        _scan_device_results.emplace(base_topic, msg->get_payload_str());
        cv.notify_all();
    });

    // Request for device scan
    _publish("pza", "d");
    ret = cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&](void) {
        return (_scan_device_count_expected && (_scan_device_count_expected == _scan_device_results.size()));
    });
    _unsubscribe("pza/+/+/device/atts/info");

    // Process timeout error
    if (ret == false) {
        spdlog::error("Device scan timed out");
        spdlog::debug("Expected {} devices, got {}", _scan_device_count_expected, _scan_device_results.size());
        return -1;
    }

    // Process success logs
    spdlog::debug("scan successful, found {} devices", _scan_device_results.size());
    if (core::get_log_level() == core::log_level::trace) {
        for (auto &it : _scan_device_results) {
            spdlog::trace("device: {}", it.first);
        }
    }
    return 0;
}

// ============================================================================
// 
int client::_scan_interfaces(std::unique_lock<std::mutex> &lock, const device::ptr &device)
{
    bool ret;
    std::condition_variable cv;
    std::string itf_topic = device->_get_base_topic() + "/+/atts/info";
    const std::string &scan_payload = _scan_device_results[device->_get_base_topic()];

    if (json::get_unsigned_int(scan_payload, "info", "number_of_interfaces", _scan_itf_count_expected) == -1) {
        spdlog::error("Unknown number of interfaces for device {}", device->_get_base_topic());
        return -1;
    }

    _scan_itf_results.clear();

    _subscribe(itf_topic, [&](const mqtt::const_message_ptr &msg) {
        std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/atts/info"));
        spdlog::trace("received interface info: {} {}", msg->get_topic(), msg->get_payload_str());
        base_topic = base_topic.substr(base_topic.find_last_of('/') + 1);
        _scan_itf_results.emplace(base_topic, msg->get_payload_str());
        cv.notify_all();
    });

    // Trigger the scan for the given device and wait for all interface info
    auto device_short_topic = device->get_group() + "/" + device->get_name();
    _publish("pza", device_short_topic);
    ret = cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&](void) {
        return (_scan_itf_count_expected && (_scan_itf_count_expected == _scan_itf_results.size()));
    });

    _unsubscribe(itf_topic);

    if (ret == false) {
        spdlog::error("timed out waiting for scan results");
        spdlog::error("_scan_itf_count_expected = {}, got = {}", _scan_itf_count_expected, _scan_itf_results.size());
        return -1;
    }

    spdlog::debug("scan successful, found {} interfaces", _scan_itf_results.size());

    if (core::get_log_level() == core::log_level::trace) {
        for (auto &it : _scan_itf_results) {
            spdlog::trace("interface: {}", it.first);
        }
    }
    return 0;
}

int client::register_device(const device::ptr &device)
{
    bool sane = false;
    bool ret;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(_mtx);

    if (device == nullptr) {
        spdlog::error("Device is null");
        return -1;
    }

    if (_devices.find(device->_get_base_topic()) != _devices.end()) {
        spdlog::warn("Device {} is already registered", device->_get_base_topic());
        return 0;
    }

    if (_scan_device_results.find(device->_get_base_topic()) == _scan_device_results.end()) {
        spdlog::error("Device {} was not scanned", device->_get_base_topic());
        return -1;
    }

    _subscribe(device->_get_device_topic() + "/atts/identity", [&](const mqtt::const_message_ptr &msg) {
        if (device->_set_identity(msg->get_payload_str()) == 0)
            sane = true;
        cv.notify_all();
    });

    ret = cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&](void) { return (sane); });
    if (ret == false) {
        spdlog::error("Device is not sane, that's very troubling");
        return -1;
    }

    if (_scan_interfaces(lock, device) == -1)
        return -1;

    device->_cli = this;

    if (device->_register_interfaces(_scan_itf_results) == -1)
        return -1;

    _devices.emplace(device->_get_base_topic(), device);
    return 0;
}

device::ptr client::create_device(const std::string &topic_str)
{
    bool recv = false;
    bool ret;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(_mtx);
    mqtt::const_message_ptr identify_msg;
    std::string family;
    topic t(topic_str);

    if (t.is_valid() == false) {
        spdlog::error("Invalid topic {}", topic_str);
        return nullptr;
    }

    _subscribe(topic_str + "/device/atts/identity", [&](const mqtt::const_message_ptr &msg) {
        identify_msg = msg;
        recv = true;
        cv.notify_all();
    });

    ret = cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&](void) { return (recv); });
    if (ret == false) {
        spdlog::error("Device is not sane, that's very troubling");
        return nullptr;
    }

    if (json::get_string(identify_msg->get_payload_str(), "identity", "family", family) == -1) {
        spdlog::error("Failed to get family from device");
        return nullptr;
    }

    device::ptr dev = device_factory::create_device(family, t.get_group(), t.get_device());

    if (dev == nullptr) {
        spdlog::error("Failed to create device");
        return nullptr;
    }

    if (dev->_set_identity(identify_msg->get_payload_str()) == -1) {
        spdlog::error("Failed to set identity");
        return nullptr;
    }

    if (_scan_interfaces(lock, dev) == -1)
        return nullptr;

    dev->_cli = this;
    
    if (dev->_register_interfaces(_scan_itf_results) == -1)
        return nullptr;

    _devices.emplace(dev->_get_base_topic(), dev);
    return dev;
}


int client::register_all_devices()
{
    int ret = 0;

    for (auto &it : _scan_device_results) {
        if (create_device(it.first) == nullptr)
            ret = -1;
    }
    return ret;
}

device::ptr client::find_device(const std::string &group, const std::string &name)
{
    std::string base_topic = "pza/" + group + "/" + name;

    if (_devices.find(base_topic) == _devices.end())
        return nullptr;
    return _devices[base_topic];
}