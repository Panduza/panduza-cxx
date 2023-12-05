#include <unistd.h>

#include <pza/core/core.hxx>
#include <pza/core/client.hxx>
#include <pza/core/interface.hxx>

#include <pza/interfaces/device.hxx>
#include <pza/interfaces/voltmeter.hxx>
#include <pza/interfaces/bps_chan_ctrl.hxx>

#include <spdlog/spdlog.h>

int main()
{

    pza::core::set_log_level(pza::core::log_level::trace);

    pza::client::s_ptr cli = std::make_shared<pza::client>("localhost", 1883);

    if (cli->connect() < 0)
        return -1;

    if (cli->register_devices() < 0)
        return -1;

    auto dev = cli->get_device("default", "coucou");
    if (dev == nullptr) {
        spdlog::error("device not found");
        return -1;
    }

    for (auto &itf : dev->get_interface_names()) {
        spdlog::info("interface: {}", itf);
    }

    auto vm = std::dynamic_pointer_cast<pza::itf::bps_chan_ctrl>(dev->get_interface("channel", 0, "ctrl"));

    if (vm) {
        spdlog::info("voltmeter found");
        spdlog::info("min voltage: {}", vm->get_min_voltage());
        spdlog::info("max voltage: {}", vm->get_max_voltage());
        spdlog::info("num decimals: {}", vm->get_num_decimals_voltage());
    }
    else {
        spdlog::error("voltmeter not found");
        return -1;
    }

    vm->set_voltage(8);

    return 0;

}
