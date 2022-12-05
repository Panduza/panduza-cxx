#include "device.hxx"
#include <pza/core/client.hxx>

using namespace pza;

device::device(const std::string &group, const std::string &name)
    : _name(name),
    _group(group),
    _base_topic("pza/" + group + "/" + name),
    _device_topic(_base_topic + "/device")
{

}

void device::reset()
{
    _state = state::orphan;
    _model = "";
    _manufacturer = "";
}

int device::_set_identity(const std::string &payload)
{
    std::string family;

    if (json::get_string(payload, "identity", "model", _model) == -1) {
        spdlog::error("Device does not have a model");
        return -1;
    }

    if (json::get_string(payload, "identity", "manufacturer", _manufacturer) == -1) {
        spdlog::error("Device does not have a manufacturer");
        return -1;
    }

    if (json::get_string(payload, "identity", "family", family) == -1) {
        spdlog::error("Device does not have a family");
        return -1;
    }

    // Convert to lowercase
    std::transform(family.begin(), family.end(), family.begin(), ::tolower);

    if (family != get_family()) {
        spdlog::error("Device is not compatible {} != {}", family, get_family());
        return -1;
    }

    return 0;
}

void device::register_interface(interface &interface)
{
    _interfaces[interface._name] = &interface;
}