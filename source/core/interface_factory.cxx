#include "interface_factory.hxx"

#include <pza/core/device.hxx>
#include <pza/interfaces/bps_chan_ctrl.hxx>
#include <pza/interfaces/meter.hxx>

using namespace pza;

template <typename T>
static itf::s_ptr allocate_interface(device &dev, const std::string &name)
{
    return std::make_shared<T>(dev, name);
}

static std::unordered_map<std::string, interface_factory::factory_function> factory_map = {
    { "ammeter", allocate_interface<meter> },
    { "voltmeter", allocate_interface<meter> },
    { "bpc", allocate_interface<bps_chan_ctrl> }
};

itf::s_ptr interface_factory::create_interface(device &dev, const std::string &name, const std::string &type)
{
    static const std::vector<std::string> exclude = { "platform", "device" };

    if (std::find(exclude.begin(), exclude.end(), type) != exclude.end())
        return nullptr;

    auto it = factory_map.find(type);

    if (it == factory_map.end()) {
        spdlog::error("Unknown interface type {}", type);
        return nullptr;
    }
    return it->second(dev, name);
}
