#pragma once

#include <functional>
#include <memory>
#include <set>

#include <pza/core/interface.hxx>

struct device_impl;
class mqtt_service;
struct device_info;

namespace pza
{
class client;

class device
{
public:
	using s_ptr = std::shared_ptr<device>;
	using u_ptr = std::unique_ptr<device>;
	using w_ptr = std::weak_ptr<device>;

	explicit device(mqtt_service *mqtt, struct device_info &info);
	device(const device &) = delete;
	device &operator=(const device &) = delete;
	device(device &&) = delete;
	device &operator=(device &&) = delete;
	~device();

	[[nodiscard]] const std::string &get_name() const;
	[[nodiscard]] const std::string &get_group() const;
	[[nodiscard]] const std::string &get_model() const;
	[[nodiscard]] const std::string &get_manufacturer() const;
	[[nodiscard]] const std::string &get_family() const;

	[[nodiscard]] unsigned int get_number_of_interfaces() const;

	[[nodiscard]] itf_base::s_ptr get_interface(const std::string &name) const;

	template <typename T> std::shared_ptr<T> get_interface(const std::string &name) const
	{
		return std::dynamic_pointer_cast<T>(get_interface(name));
	}

	[[nodiscard]] itf_base::s_ptr get_interface(const std::string &interface_group, unsigned int idx,
						    const std::string &name) const;

	template <typename T>
	std::shared_ptr<T> get_interface(const std::string &interface_group, unsigned int idx,
					 const std::string &name) const
	{
		return std::dynamic_pointer_cast<T>(get_interface(interface_group, idx, name));
	}

	[[nodiscard]] std::vector<std::string> get_interfaces_name() const;

	[[nodiscard]] std::vector<itf_base::s_ptr> get_interfaces_in_group(const std::string &group) const;

	[[nodiscard]] std::vector<itf_base::s_ptr> get_interfaces_in_group(const std::string &group,
									   unsigned int index) const;

	[[nodiscard]] std::set<std::string> get_interface_groups() const;

	[[nodiscard]] size_t get_interface_groups_count() const;

	[[nodiscard]] std::vector<itf_base::s_ptr> get_interfaces() const;

private:
	std::unique_ptr<device_impl> _impl;
};
};
