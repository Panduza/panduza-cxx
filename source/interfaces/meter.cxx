#include <pza/interfaces/meter.hxx>

#include "../core/attribute.hxx"

using namespace pza;

meter::meter(device *dev, const std::string &name, client_callbacks cb)
    : itf(dev, name, cb)
{
    _measure = new attribute("measure");

    _measure->add_ro_field<double>("value");
    _measure->add_ro_field<double>("polling_cycle");

    add_attributes({_measure});
}

meter::~meter()
{
    delete _measure;
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
