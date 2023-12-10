#pragma once

#include <pza/core/interface.hxx>

namespace pza::itf
{
class device : public itf_base
{
public:
	using s_ptr = std::shared_ptr<device>;
	using u_ptr = std::unique_ptr<device>;
	using w_ptr = std::weak_ptr<device>;

	explicit device(mqtt_service *mqtt, itf_info &info);
	device(const device &) = delete;
	device(device &&) = delete;
	device &operator=(const device &) = delete;
	device &operator=(device &&) = delete;
	~device() override;

	const std::string &get_model();
	const std::string &get_manufacturer();
	const std::string &get_family();

private:
	attribute_ptr _identity;
};
};
