#include <pza/interfaces/meter.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

meter::meter(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _measure = std::make_unique<attribute>("measure");

    _measure->add_ro_field<double>("value");
    _measure->add_ro_field<double>("polling_cycle");

    register_attributes({*_measure});
}

meter::~meter()
{

}

double meter::get_measure()
{
    return _measure->get_field<double>("value").get();
}

int meter::set_measure_polling_cycle(double seconds)
{
    return _measure->get_field<double>("polling_cycle").set(seconds);
}

void meter::add_measure_callback(const std::function<void(double)> &callback)
{
    _measure->get_field<double>("value").add_get_callback(callback);
}

void meter::remove_measure_callback(const std::function<void(double)> &callback)
{
    _measure->get_field<double>("value").remove_get_callback(callback);
}
