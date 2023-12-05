#include <pza/interfaces/device.hxx>

#include "../core/interface.hxx"

using namespace pza::itf;

device::device(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _identity = std::make_shared<attribute>("identity");

    _identity->register_field<std::string>("model");
    _identity->register_field<std::string>("manufacturer");
    _identity->register_field<std::string>("family");

    _impl->register_attributes({_identity});
}

device::~device()
{
    spdlog::info("device::~device");
}

const std::string &device::get_model(void)
{
    return _identity->get_field<std::string>("model");
}

const std::string &device::get_manufacturer(void)
{
    return _identity->get_field<std::string>("manufacturer");
}

const std::string &device::get_family(void)
{
    return _identity->get_field<std::string>("family");
}
