#include <pza/interfaces/device.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

device::device(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _identity = std::make_unique<attribute>("identity");

    _identity->register_field<std::string>("family");

    register_attributes({*_identity});
}

device::~device()
{

}

std::string device::get_family()
{
    return _identity->get_field<std::string>("family");
}

std::string device::get_model()
{
    return _identity->get_field<std::string>("model");
}

std::string device::get_manufacturer()
{
    return _identity->get_field<std::string>("manufacturer");
}
