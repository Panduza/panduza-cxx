#pragma once

#include <pza/interfaces/meter.hxx>

namespace pza::itf
{
class voltmeter : public meter
{
public:
	using s_ptr = std::shared_ptr<voltmeter>;
	using u_ptr = std::unique_ptr<voltmeter>;
	using w_ptr = std::weak_ptr<voltmeter>;

	explicit voltmeter(mqtt_service &mqtt, itf_info &info);
	voltmeter(const voltmeter &) = delete;
	voltmeter(voltmeter &&) = delete;
	voltmeter &operator=(const voltmeter &) = delete;
	voltmeter &operator=(voltmeter &&) = delete;
	~voltmeter() override;
};
};
