#include <pza/interfaces/platform.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

platform::platform(mqtt_service &mqtt, itf_info &info) : itf_base(mqtt, info)
{
}

platform::~platform() = default;
