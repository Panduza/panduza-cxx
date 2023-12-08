#include <pza/interfaces/ammeter.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

ammeter::ammeter(mqtt_service &mqtt, itf_info &info) : meter(mqtt, info)
{
}

ammeter::~ammeter() = default;
