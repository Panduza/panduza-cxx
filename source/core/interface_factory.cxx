#include "interface_factory.hxx"

#include "interfaces/bps_chan_ctrl.hxx"
#include "interfaces/meter.hxx"

static std::map<std::string, interface_factory::factory_function> factory_map = {
    { "ammeter", interface_factory::allocate_interface<meter> },
    { "voltmeter", interface_factory::allocate_interface<meter> },
    { "bpc", interface_factory::allocate_interface<bps_chan_ctrl> }
};

itf::ptr interface_factory::create_interface(device *dev, const std::string &name, const std::string &type)
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