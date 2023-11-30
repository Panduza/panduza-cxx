#include <string>
#include <mutex>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include <pza/core/client.hxx>
#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include <pza/interfaces/device.hxx>

#include "../utils/json_attribute.hxx"
#include "interface_factory.hxx"
#include "mqtt_service.hxx"
#include "scanner.hxx"

using namespace pza;

struct device_impl
{
    explicit device_impl(mqtt_service &mqtt, const struct device_info &info);

    void on_interface_info(mqtt::const_message_ptr msg);

    struct device_info info;
    std::unordered_map<std::string, std::string> interfaces_scanned;
    std::unordered_map<std::string, itf_base::s_ptr> interfaces;
    itf::device::s_ptr device_interface = nullptr;
};

device_impl::device_impl(mqtt_service &mqtt, const struct device_info &info)
    : info(info)
{
    scanner scanner(mqtt);

    scanner.set_scan_timeout(5)
        .set_message_callback(std::bind(&device_impl::on_interface_info, this, std::placeholders::_1))
        .set_condition_callback([&](void) {
            return (info.number_of_interfaces && (info.number_of_interfaces == interfaces_scanned.size()));
        })
        .set_publisher(mqtt::make_message("pza", info.group + "/" + info.name))
        .set_subscription_topic("pza/" + info.group + "/" + info.name + "/+/atts/info");

    if (scanner.run() < 0)
        spdlog::error("timed out waiting for interfaces, expected {} got {}", info.number_of_interfaces, interfaces_scanned.size());
}

void device_impl::on_interface_info(mqtt::const_message_ptr msg)
{
    std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/atts/info"));
    std::string itf_name = base_topic.substr(base_topic.find_last_of('/') + 1);

    spdlog::trace("received interface info: {} {}", msg->get_topic(), msg->get_payload_str());

    interfaces_scanned[itf_name] = msg->get_payload_str();
}

device::device(mqtt_service &mqtt, struct device_info &info)
    : _impl(std::make_unique<device_impl>(mqtt, info))
{
    json_attribute json("info");

    for (auto &itf : _impl->interfaces_scanned) {
        std::string type;

        if (json.parse(itf.second) < 0) {
            spdlog::error("failed to parse attribute info for interface {}", itf.first);
            continue;
        }

        if (json.get_string("type", type) < 0) {
            spdlog::error("failed to get type for interface {}", itf.first);
            continue;
        }

        auto itf_ptr = interface_factory::create_interface(mqtt, info.group, info.name, itf.first, type);
        if (itf_ptr == nullptr) {
            spdlog::error("failed to create interface {} of type {}", itf.first, type);
            continue;
        }
        _impl->interfaces[itf.first] = itf_ptr;
    }

    if (_impl->interfaces.find("device") != _impl->interfaces.end()) {
        _impl->device_interface = std::static_pointer_cast<itf::device>(_impl->interfaces["device"]);
        info.family = _impl->device_interface->get_family();
        info.manufacturer = _impl->device_interface->get_manufacturer();
        info.model = _impl->device_interface->get_model();

    }
    else {
        throw std::runtime_error("device interface not found");
    }
}

device::~device()
{

}

const std::string &device::get_name() const
{
    return _impl->info.name;
}

const std::string &device::get_group() const
{
    return _impl->info.group;
}   

const std::string &device::get_model() const
{
    return _impl->info.model;
}

const std::string &device::get_manufacturer() const
{
    return _impl->info.manufacturer;
}

const std::string &device::get_family() const
{
    return _impl->info.family;
}