#include "device_factory.hxx"

#include <pza/devices/bps.hxx>
#include <pza/devices/orphan.hxx>


using namespace pza;

std::map<std::string, device_factory::factory_function> device_factory::_factory_map = {
    { "bps", device_factory::allocate_device<bps> }
};

device::ptr device_factory::create_device(const std::string &family, const std::string &group, const std::string &name)
{
    auto it = _factory_map.find(family);
    if (it == _factory_map.end()) {
        spdlog::error("Unknown device type {}", family);

        return device_factory::allocate_device<orphan>(group, name);
    }

    return it->second(group, name);
}