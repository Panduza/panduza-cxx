#pragma once

#include <pza/interfaces/meter.hxx>

namespace pza::itf
{
class ammeter : public meter
{
public:
	using s_ptr = std::shared_ptr<ammeter>;
	using u_ptr = std::unique_ptr<ammeter>;
	using w_ptr = std::weak_ptr<ammeter>;

	explicit ammeter(mqtt_service *mqtt, itf_info &info);
	ammeter(const ammeter &) = delete;
	ammeter(ammeter &&) = delete;
	ammeter &operator=(const ammeter &) = delete;
	ammeter &operator=(ammeter &&) = delete;
	~ammeter() override;
};
};
