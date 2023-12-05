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

    auto dev = cli->get_device("default", "Panduza_FakeBps");
    if (dev)
        spdlog::info("device {} found", dev->get_name());
    else {
        spdlog::error("device not found");
        return -1;
    }

    return 0;

}
