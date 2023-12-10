#pragma once

#include <pza/core/interface.hxx>

namespace pza::itf
{
class bps_chan_ctrl : public itf_base
{
public:
	using s_ptr = std::shared_ptr<bps_chan_ctrl>;
	using u_ptr = std::unique_ptr<bps_chan_ctrl>;
	using w_ptr = std::weak_ptr<bps_chan_ctrl>;

	explicit bps_chan_ctrl(mqtt_service *mqtt, itf_info &info);
	bps_chan_ctrl(const bps_chan_ctrl &) = delete;
	bps_chan_ctrl(bps_chan_ctrl &&) = delete;
	bps_chan_ctrl &operator=(const bps_chan_ctrl &) = delete;
	bps_chan_ctrl &operator=(bps_chan_ctrl &&) = delete;
	~bps_chan_ctrl() override;

	int set_voltage(double volts);
	int set_current(double amps);
	int set_enable(bool enable);

	bool get_enable();
	double get_min_voltage();
	double get_max_voltage();
	double get_min_current();
	double get_max_current();
	double get_preset_voltage();
	double get_preset_current();
	unsigned int get_num_decimals_voltage();
	unsigned int get_num_decimals_current();

	void register_enable_callback(const std::function<void()> &callback);
	void remove_enable_callback(const std::function<void()> &callback);

private:
	attribute_ptr _voltage;
	attribute_ptr _current;
	attribute_ptr _enable;
};
};
