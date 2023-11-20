#include "scanner.hxx"

scanner::scanner(const struct client_callbacks &callbacks)
    : _callbacks(callbacks)
{

}

int scanner::scan()
{
    int ret;

    ret = _scan_platforms();
    if (ret < 0)
        return ret;

    return _scan_devices();
}

int scanner::_scan_platforms()
{
    std::unique_lock<std::mutex> lock(_mtx);
    const std::string &topic = "pza/server/+/+/atts/info";

    _platforms.clear();
    _device_count = 0;

    _callbacks.subscribe(topic, std::bind(&scanner::_on_platform_info, this, std::placeholders::_1));
    _callbacks.publish("pza", "p");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    _callbacks.unsubscribe(topic);

    if (_platforms.empty()) {
        spdlog::error("No platforms found");
        return -1;
    }

    if (_device_count == 0) {
        spdlog::error("No devices found on scanned platforms");
        return -1;
    }
    spdlog::info("Found {} platform{} with {} devices", _platforms.size(), _platforms.size() > 1 ? "s" : "", _device_count);
    return 0;
}

void scanner::_on_platform_info(mqtt::const_message_ptr msg)
{
    const std::string &payload = msg->get_payload_str();
    const std::string &topic = msg->get_topic();
    std::string type;
    unsigned int val;

    if (json::get_string(payload, "info", "type", type) < 0) {
        spdlog::error("failed to parse type info: {}", payload);
        return;
    }
    if (type != "platform" || _platforms.find(topic) != _platforms.end())
        return;

    _platforms.insert(topic);

    spdlog::trace("received platform info: {}", payload);

    if (json::get_unsigned_int(payload, "info", "number_of_devices", val) < 0) {
        spdlog::error("failed to parse platform info: {}", payload);
        return;
    }
    _device_count += val;
}

int scanner::_scan_devices()
{
    std::unique_lock<std::mutex> lock(_mtx);
    const std::string &topic = "pza/+/+/device/atts/info";
    bool ret;

    _callbacks.subscribe(topic, std::bind(&scanner::_on_device_info, this, std::placeholders::_1));
    _callbacks.publish("pza", "d");

    ret = _cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&] { return _device_count == _devices.size(); });

    _callbacks.unsubscribe(topic);

    if (ret == false) {
        spdlog::error("timed out waiting for devices");
        return -1;
    }
    return 0;
}

void scanner::_on_device_info(mqtt::const_message_ptr msg)
{
    std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/device/atts/info"));

    spdlog::trace("received device info: {} {}", msg->get_topic(), msg->get_payload_str());
    _devices.emplace(base_topic, msg->get_payload_str());
    _cv.notify_one();
}

int scanner::scan_interfaces(const std::string &group, const std::string &name)
{
    std::unique_lock<std::mutex> lock(_mtx);
    const std::string &short_topic = group + "/" + name;
    const std::string &base_topic = "pza/" + short_topic;
    const std::string &itf_topic = base_topic + "/+/atts/info";
    bool ret;
    
    if (_devices.count(base_topic) == 0) {
        spdlog::error("device {} was not scanned", base_topic);
        return -1;
    }

    if (json::get_unsigned_int(_devices[base_topic], "info", "number_of_interfaces", _interface_count) < 0) {
        spdlog::error("Unknown number of interfaces for device {}", base_topic);
        return -1;
    }

    _interfaces.clear();

    _callbacks.subscribe(itf_topic, std::bind(&scanner::_on_interface_info, this, std::placeholders::_1));
    _callbacks.publish("pza", short_topic);

    ret = _cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&](void) {
        return (_interface_count && (_interface_count == _interfaces.size()));
    });

    _callbacks.unsubscribe(itf_topic);

    if (ret == false) {
        spdlog::error("timed out waiting for interfaces");
        ret = -1;
    }
    return 0;
}

void scanner::_on_interface_info(mqtt::const_message_ptr msg)
{
    std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/atts/info"));
    std::string itf_name = base_topic.substr(base_topic.find_last_of('/') + 1);

    spdlog::trace("received interface info: {} {}", msg->get_topic(), msg->get_payload_str());

    _interfaces.emplace(itf_name, msg->get_payload_str());
    _cv.notify_one();
}

int scanner::scan_device_identity(const std::string &group, const std::string &name)
{
    std::unique_lock<std::mutex> lock(_mtx);
    const std::string &topic = "pza/" + group + "/" + name + "/device/atts/identity";
    bool ret;

    _device_identity = "";
    _callbacks.subscribe(topic, std::bind(&scanner::_on_identity_info, this, std::placeholders::_1));

    ret = _cv.wait_for(lock, std::chrono::seconds(_scan_timeout), [&] { return (_device_identity.empty() == false); });

    _callbacks.unsubscribe(topic);

    if (ret == false) {
        spdlog::error("timed out waiting for identity message");
        return -1;
    }
    return 0;
}

void scanner::_on_identity_info(mqtt::const_message_ptr msg)
{
    spdlog::trace("received device identity: {} {}", msg->get_topic(), msg->get_payload_str());
    _device_identity = msg->get_payload_str();
    _cv.notify_one();
}

bool scanner::device_was_scanned(const std::string &group, const std::string &name)
{
    return (_devices.count("pza/" + group + "/" + name) > 0);
}