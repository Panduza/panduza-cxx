#include "interface_factory.hxx"

#include <pza/core/device.hxx>

#include <pza/interfaces/ammeter.hxx>
#include <pza/interfaces/bps_chan_ctrl.hxx>
#include <pza/interfaces/device.hxx>
#include <pza/interfaces/platform.hxx>
#include <pza/interfaces/voltmeter.hxx>

template <typename T> static itf_base::s_ptr allocate_interface(mqtt_service *mqtt, itf_info &info)
{
	return std::make_shared<T>(mqtt, info);
}

static const std::unordered_map<std::string, interface_factory::factory_function> factory_map = {
    {"device", allocate_interface<itf::device>},
    {"platform", allocate_interface<itf::platform>},
    {"ammeter", allocate_interface<itf::ammeter>},
    {"voltmeter", allocate_interface<itf::voltmeter>},
    {"bpc", allocate_interface<itf::bps_chan_ctrl>}};

itf_base::s_ptr interface_factory::create_interface(mqtt_service *mqtt, const std::string &group,
						    const std::string &device_name, const std::string &name,
						    const std::string &type)
{
	itf_info info;

	info.name = name;
	info.group = group;
	info.device_name = device_name;
	info.type = type;

	auto it = factory_map.find(type);

	if (it == factory_map.end()) {
		spdlog::error("Unknown interface type {}", type);
		return nullptr;
	}
	return it->second(mqtt, info);
}
