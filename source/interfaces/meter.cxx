#include <pza/interfaces/meter.hxx>

#include "../core/interface.hxx"

using namespace pza::itf;

meter::meter(mqtt_service &mqtt, itf_info &info) : itf_base(mqtt, info)
{
	_measure = std::make_shared<attribute>("measure");

	_measure->register_field<double>("value");

	_impl->register_attributes({_measure});
}

meter::~meter() = default;

double meter::get_measure()
{
	return _impl->get_attribute("measure").get_field<double>("value");
}

void meter::register_measure_callback(const std::function<void()> &callback)
{
	_impl->get_attribute("measure").register_callback(callback);
}

void meter::remove_measure_callback(const std::function<void()> &callback)
{
	_impl->get_attribute("measure").remove_callback(callback);
}
