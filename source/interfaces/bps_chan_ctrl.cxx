#include <pza/interfaces/bps_chan_ctrl.hxx>

#include "../core/interface.hxx"

using namespace pza::itf;

bps_chan_ctrl::bps_chan_ctrl(mqtt_service *mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
	_enable = std::make_shared<attribute>("enable");
	_voltage = std::make_shared<attribute>("voltage");
	_current = std::make_shared<attribute>("current");

	_enable->register_field<bool>("value");

	_voltage->register_field<double>("value");
	_voltage->register_field<double>("min");
	_voltage->register_field<double>("max");
	_voltage->register_field<unsigned int>("decimals");

	_current->register_field<double>("value");
	_current->register_field<double>("min");
	_current->register_field<double>("max");
	_current->register_field<unsigned int>("decimals");

	_impl->register_attributes({_enable, _voltage, _current});
}

bps_chan_ctrl::~bps_chan_ctrl() = default;

int bps_chan_ctrl::set_voltage(double volts)
{
	auto min = _voltage->get_field<double>("min");
	auto max = _voltage->get_field<double>("max");

	if (volts < min || volts > max) {
		spdlog::error("bps_chan_ctrl::set_voltage: voltage out of "
			      "range, min: {}, max: {}, value: {}",
			      min, max, volts);
		return -1;
	}
	return _voltage->set_field<double>("value", volts);
}

int bps_chan_ctrl::set_current(double amps)
{
	auto min = _current->get_field<double>("min");
	auto max = _current->get_field<double>("max");

	if (amps < min || amps > max) {
		spdlog::error("bps_chan_ctrl::set_current: current out of "
			      "range, min: {}, max: {}, value: {}",
			      min, max, amps);
		return -1;
	}
	return _current->set_field<double>("value", amps);
}

int bps_chan_ctrl::set_enable(bool enable) { return _enable->set_field<bool>("value", enable); }

bool bps_chan_ctrl::get_enable() { return _enable->get_field<bool>("value"); }

double bps_chan_ctrl::get_min_voltage() { return _voltage->get_field<double>("min"); }

double bps_chan_ctrl::get_max_voltage() { return _voltage->get_field<double>("max"); }

double bps_chan_ctrl::get_min_current() { return _current->get_field<double>("min"); }

double bps_chan_ctrl::get_max_current() { return _current->get_field<double>("max"); }

double bps_chan_ctrl::get_preset_voltage() { return _voltage->get_field<double>("value"); }

double bps_chan_ctrl::get_preset_current() { return _current->get_field<double>("value"); }

unsigned int bps_chan_ctrl::get_num_decimals_voltage() { return _voltage->get_field<unsigned int>("decimals"); }

unsigned int bps_chan_ctrl::get_num_decimals_current() { return _current->get_field<unsigned int>("decimals"); }

void bps_chan_ctrl::register_enable_callback(const std::function<void()> &callback)
{
	_enable->register_callback(callback);
}

void bps_chan_ctrl::remove_enable_callback(const std::function<void()> &callback)
{
	_enable->remove_callback(callback);
}
