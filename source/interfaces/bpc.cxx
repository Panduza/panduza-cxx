#include <pza/interfaces/bpc.hxx>

#include "../core/interface.hxx"

using namespace pza::itf;

bpc::bpc(mqtt_service *mqtt, itf_info &info)
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

bpc::~bpc() = default;

int bpc::set_voltage(double volts)
{
	auto min = _voltage->get_field<double>("min");
	auto max = _voltage->get_field<double>("max");

	if (volts < min || volts > max) {
		spdlog::error("bpc::set_voltage: voltage out of "
			      "range, min: {}, max: {}, value: {}",
			      min, max, volts);
		return -1;
	}
	return _voltage->set_field<double>("value", volts);
}

int bpc::set_current(double amps)
{
	auto min = _current->get_field<double>("min");
	auto max = _current->get_field<double>("max");

	if (amps < min || amps > max) {
		spdlog::error("bpc::set_current: current out of "
			      "range, min: {}, max: {}, value: {}",
			      min, max, amps);
		return -1;
	}
	return _current->set_field<double>("value", amps);
}

int bpc::set_enable(bool enable)
{
	return _enable->set_field<bool>("value", enable);
}

bool bpc::get_enable()
{
	return _enable->get_field<bool>("value");
}

double bpc::get_min_voltage()
{
	return _voltage->get_field<double>("min");
}

double bpc::get_max_voltage()
{
	return _voltage->get_field<double>("max");
}

double bpc::get_min_current()
{
	return _current->get_field<double>("min");
}

double bpc::get_max_current()
{
	return _current->get_field<double>("max");
}

double bpc::get_preset_voltage()
{
	return _voltage->get_field<double>("value");
}

double bpc::get_preset_current()
{
	return _current->get_field<double>("value");
}

unsigned int bpc::get_num_decimals_voltage()
{
	return _voltage->get_field<unsigned int>("decimals");
}

unsigned int bpc::get_num_decimals_current()
{
	return _current->get_field<unsigned int>("decimals");
}

void bpc::register_enable_callback(const std::function<void()> &callback)
{
	_enable->register_callback(callback);
}

void bpc::remove_enable_callback(const std::function<void()> &callback)
{
	_enable->remove_callback(callback);
}
