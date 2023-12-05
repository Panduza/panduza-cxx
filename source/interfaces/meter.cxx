#include <pza/interfaces/meter.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

meter::meter(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _measure = std::make_unique<attribute>("measure");

    _measure->register_field<double>("value");

    register_attributes({*_measure});
}

meter::~meter()
{

}

double meter::get_measure(void)
{
    return _measure->get_field<double>("value");
}

void meter::register_measure_callback(const std::function<void()> &callback)
{
    _measure->register_callback(callback);
}

void meter::remove_measure_callback(const std::function<void()> &callback)
{
    _measure->remove_callback(callback);
}