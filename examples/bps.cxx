#include <pza/core/core.hxx>
#include <pza/core/client.hxx>
#include <pza/interfaces/voltmeter.hxx>
#include <pza/interfaces/ammeter.hxx>
#include <pza/interfaces/bps_chan_ctrl.hxx>

int main(void)
{
    spdlog::info("pza version: {}", pza::core::get_version());
    spdlog::info("pza version hash: {}", pza::core::get_version_githash());
    pza::core::set_log_level(pza::core::log_level::debug);

    pza::client::s_ptr cli = std::make_shared<pza::client>("localhost", 1883);

    if (cli->connect() < 0)
        return -1;

    if (cli->register_devices() < 0)
        return -1;

    auto vm = cli->get_interface<pza::itf::voltmeter>("default", "Panduza_FakeBps", "channel", 0, "vm");
    if (!vm) {
        spdlog::error("voltmeter not found");
        return -1;
    }

    auto ctrl = cli->get_interface<pza::itf::bps_chan_ctrl>("default", "Panduza_FakeBps", "channel", 0, "ctrl");
    if (!ctrl) {
        spdlog::error("ctrl not found");
        return -1;
    }

    auto am = cli->get_interface<pza::itf::ammeter>("default", "Panduza_FakeBps", "channel", 0, "am");
    if (!am) {
        spdlog::error("ammeter not found");
        return -1;
    }

    ctrl->set_enable(true);
    ctrl->set_voltage(3.3);
    ctrl->set_current(0.1);

    vm->register_measure_callback([&]() {
        spdlog::info("voltage: {}", vm->get_measure());
    });

    am->register_measure_callback([&]() {
        spdlog::info("current: {}", am->get_measure());
    });

    while (1)
        ;
    return 0;
}
