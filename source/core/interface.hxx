#include <mutex>
#include <string>

#include <mqtt/message.h>

#include <pza/core/interface.hxx>

#include "attribute.hxx"
#include "mqtt_service.hxx"

using namespace pza;

struct itf_info {
	std::string group;
	std::string device_name;
	std::string name;
	std::string type;
};

struct itf_impl {
	explicit itf_impl(mqtt_service *mqtt, itf_info &info);

	virtual ~itf_impl();
	itf_impl(const itf_impl &) = delete;
	itf_impl &operator=(const itf_impl &) = delete;
	itf_impl(itf_impl &&) = delete;
	itf_impl &operator=(itf_impl &&) = delete;

	const std::string &get_name() const
	{
		return info.name;
	}
	const std::string &get_type() const
	{
		return info.type;
	}

	void register_attributes(const std::list<attribute::s_ptr> &attribute_list);

	attribute &get_attribute(const std::string &name);

	int send_message(const nlohmann::json &data);

	itf_info info;
	std::string topic_base;
	std::string topic_cmd;
	mqtt_service *mqtt;
	std::unordered_map<std::string, attribute::s_ptr> attributes;
};
