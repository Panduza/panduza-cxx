#include "device.hxx"
#include "client.hxx"

device::device(client *cli, const std::string &group, const std::string &name)
    : _cli(cli),
    _name(name),
    _group(group),
    _base_topic("pza/" + group + "/" + name),
    _device_topic(_base_topic + "/device")
{
    device::ptr dev;
    itf::ptr itf;
    scanner &_scanner = _cli->get_scanner();

    if (_scanner.device_was_scanned(group, name) == false)
        throw std::runtime_error("device was not scanned");

    if (_scanner.scan_device_identity(group, name) < 0)
        throw std::runtime_error("failed to scan identity for device");

    if (json::get_string(_scanner.get_device_identity(), "identity", "model", _model) < 0)
        throw std::runtime_error("Device does not have a model");

    if (json::get_string(_scanner.get_device_identity(), "identity", "manufacturer", _manufacturer) < 0)
        throw std::runtime_error("Device does not have a manufacturer");

    if (json::get_string(_scanner.get_device_identity(), "identity", "family", _family) < 0)
        throw std::runtime_error("Device does not have a family");

    if (_scanner.scan_interfaces(group, name) < 0)
        throw std::runtime_error("failed to scan interfaces for device");

    std::transform(_family.begin(), _family.end(), _family.begin(), ::tolower);
    create_interfaces(_scanner.get_interfaces());
}

void device::create_interfaces(const scanner::interface_map &interfaces)
{
    for (const auto &elem : interfaces) {
        itf::ptr new_itf = interface_factory::create_interface(this, elem.first, elem.second);

        if (new_itf == nullptr || _register_interface(new_itf) < 0) {
            spdlog::error("failed to create interface {}", elem.first);
            continue;
        }
    }
}

int device::_register_interface(itf::ptr interface)
{
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(_mtx);
    bool received = false;
    bool ret;

    for (auto const &attribute: interface->get_attributes()) {
        std::string topic;

        topic = interface->get_topic_base() + "/atts/" + attribute.first;
        _cli->subscribe(topic, [&](const mqtt::const_message_ptr &msg) {
            attribute.second->on_message(msg);
            cv.notify_one();
            received = true;
        });
        ret = cv.wait_for(lock, std::chrono::seconds(_cli->get_scanner().get_scan_timeout()), [&]() { return received; });
        _cli->unsubscribe(topic);
        if (ret == false) {
            spdlog::error("timed out waiting for attribute registration on {}", topic);
            return -1;
        }
        _cli->subscribe(topic, std::bind(&attribute::on_message, attribute.second, std::placeholders::_1));
    }
    _interfaces[interface->get_name()] = interface;
    return 0;
}
