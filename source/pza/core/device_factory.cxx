#include "device_factory.hxx"

using namespace pza;


device::ptr device_factory::create_device(const std::string &family, const std::string &group, const std::string &name)
{
    static std::map<std::string, device_factory::factory_function> factory_map = {
        { "bps", device_factory::allocate_device<bps> }
    };
    std::string family_lower = family;

    std::transform(family_lower.begin(), family_lower.end(), family_lower.begin(), ::tolower);
    auto it = factory_map.find(family_lower);
    if (it == factory_map.end()) {
        spdlog::error("Unknown device type {}", family);
        return nullptr;
    }

    return it->second(group, name);
}