#include <pza/interfaces/device.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

device::device(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _identity = std::make_unique<attribute>("identity");

    _identity->add_ro_field<std::string>("family");
    _identity->add_ro_field<std::string>("model");
    _identity->add_ro_field<std::string>("manufacturer");

    register_attributes({*_identity});
}

device::~device()
{

}
