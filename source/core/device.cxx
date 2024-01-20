#include <map>
#include <mutex>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include <pza/core/client.hxx>
#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include <pza/core/utils.hxx>
#include <pza/interfaces/device.hxx>

#include "../utils/json_attribute.hxx"
#include "interface_factory.hxx"
#include "mqtt_service.hxx"
#include "scanner.hxx"

using namespace pza;

struct device_info {
	std::string name;
	std::string group;
	std::string model;
	std::string manufacturer;
	std::string family;
	unsigned int number_of_interfaces;
};

static constexpr unsigned int interfaces_timeout = 1000;

struct device_impl {
	explicit device_impl(mqtt_service *mqtt, const struct device_info &info);
	device_impl(const device_impl &) = delete;
	device_impl(device_impl &&) = delete;
	device_impl &operator=(const device_impl &) = delete;
	device_impl &operator=(device_impl &&) = delete;
	~device_impl();

	void on_interface_info(mqtt::const_message_ptr msg);

	unsigned int get_number_of_interfaces() const
	{
		return interfaces.size();
	}

	itf_base::s_ptr find_interface(const std::string &name) const;
	itf_base::s_ptr get_interface(const std::string &name) const;
	itf_base::s_ptr get_interface(const std::string &interface_group, unsigned int idx,
				      const std::string &name) const;

	std::vector<std::string> get_interfaces_name() const;

	std::vector<itf_base::s_ptr> get_interfaces_in_group(const std::string &group) const;
	std::vector<itf_base::s_ptr> get_interfaces_in_group(const std::string &group, unsigned int index) const;
	std::set<std::string> get_interface_groups() const;

	std::vector<itf_base::s_ptr> get_interfaces() const;

	struct device_info info;
	std::unordered_map<std::string, std::string> interfaces_scanned;
	itf::device::s_ptr device_interface = nullptr;

	std::unordered_map<std::string, itf_base::s_ptr> interfaces;
};

device_impl::device_impl(mqtt_service *mqtt, const struct device_info &info)
    : info(info)
{
	scanner scanner(mqtt);

	auto on_interface_info = [&](mqtt::const_message_ptr msg) {
		std::string base_topic = msg->get_topic().substr(0, msg->get_topic().find("/atts/info"));
		std::string itf_name = base_topic.substr(base_topic.find_last_of('/') + 1);

		spdlog::trace("received interface info: {} {}", msg->get_topic(), msg->get_payload_str());

		interfaces_scanned[itf_name] = msg->get_payload_str();
	};

	scanner.set_scan_timeout_ms(interfaces_timeout)
	    .set_message_callback(on_interface_info)
	    .set_condition_callback([&]() {
		    return (info.number_of_interfaces && (info.number_of_interfaces == interfaces_scanned.size()));
	    })
	    .set_publisher(mqtt::make_message("pza", info.group + "/" + info.name))
	    .set_subscription_topic("pza/" + info.group + "/" + info.name + "/+/atts/info");

	if (scanner.run() < 0)
		spdlog::error("timed out waiting for interfaces, expected {} got {}", info.number_of_interfaces,
			      interfaces_scanned.size());
}

device_impl::~device_impl() = default;

itf_base::s_ptr device_impl::find_interface(const std::string &name) const
{
	auto it = interfaces.find(name);

	if (it == interfaces.end()) {
		spdlog::error("interface {} not found", name);
		return nullptr;
	}
	return it->second;
}

itf_base::s_ptr device_impl::get_interface(const std::string &name) const
{
	return find_interface(name);
}

itf_base::s_ptr device_impl::get_interface(const std::string &interface_group, unsigned int idx,
					   const std::string &name) const
{
	return find_interface(utils::format_interface_group(interface_group, idx, name));
}

std::vector<std::string> device_impl::get_interfaces_name() const
{
	std::vector<std::string> names;

	for (auto &it : interfaces) {
		names.push_back(it.first);
	}
	return names;
}

std::vector<itf_base::s_ptr> device_impl::get_interfaces_in_group(const std::string &group) const
{
	std::vector<itf_base::s_ptr> interfaces_in_group;
	struct utils::itf_group_info info;

	for (const auto &itf : interfaces) {
		if (utils::get_grouped_interface_info(itf.first, info) == 0 && info.group_name == group) {
			interfaces_in_group.push_back(itf.second);
		}
	}
	return interfaces_in_group;
}

std::vector<itf_base::s_ptr> device_impl::get_interfaces_in_group(const std::string &group, unsigned int index) const
{
	std::vector<itf_base::s_ptr> interfaces_in_group;
	struct utils::itf_group_info info;

	for (const auto &itf : interfaces) {
		if (utils::get_grouped_interface_info(itf.first, info) == 0 && info.group_name == group &&
		    info.index == index) {
			interfaces_in_group.push_back(itf.second);
		}
	}
	return interfaces_in_group;
}

std::set<std::string> device_impl::get_interface_groups() const
{
	std::set<std::string> unique_groups;
	struct utils::itf_group_info info;

	for (const auto &itf : interfaces) {
		if (utils::get_grouped_interface_info(itf.first, info) == 0) {
			unique_groups.insert(info.group_name);
		}
	}
	return unique_groups;
}

std::vector<itf_base::s_ptr> device_impl::get_interfaces() const
{
	std::vector<itf_base::s_ptr> vec;

	for (const auto &itf : interfaces) {
		vec.push_back(itf.second);
	}
	return vec;
}

device::device(mqtt_service *mqtt, struct device_info &info)
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
		_impl->info.family = _impl->device_interface->get_family();
		_impl->info.manufacturer = _impl->device_interface->get_manufacturer();
		_impl->info.model = _impl->device_interface->get_model();
	} else {
		throw std::runtime_error("device interface not found");
	}
}

device::~device() = default;

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

unsigned int device::get_number_of_interfaces() const
{
	return _impl->get_number_of_interfaces();
}

itf_base::s_ptr device::get_interface(const std::string &name) const
{
	return _impl->get_interface(name);
}

itf_base::s_ptr device::get_interface(const std::string &interface_group, unsigned int idx,
				      const std::string &name) const
{
	return _impl->get_interface(interface_group, idx, name);
}

std::vector<std::string> device::get_interfaces_name() const
{
	return _impl->get_interfaces_name();
}

std::vector<itf_base::s_ptr> device::get_interfaces_in_group(const std::string &group) const
{
	return _impl->get_interfaces_in_group(group);
}

std::vector<itf_base::s_ptr> device::get_interfaces_in_group(const std::string &group, unsigned int index) const
{
	return _impl->get_interfaces_in_group(group, index);
}

std::set<std::string> device::get_interface_groups() const
{
	return _impl->get_interface_groups();
}

std::vector<itf_base::s_ptr> device::get_interfaces() const
{
	return _impl->get_interfaces();
}
