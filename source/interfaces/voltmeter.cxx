#include <pza/interfaces/voltmeter.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

voltmeter::voltmeter(mqtt_service &mqtt, itf_info &info) : meter(mqtt, info)
{
}

voltmeter::~voltmeter() = default;
