#include <pza/core/client.hxx>
#include <pza/core/core.hxx>
#include <pza/devices/bps.hxx>

int main(void)
{
    pza::core::set_log_level(pza::core::log_level::trace);

    pza::client::ptr cli = std::make_shared<pza::client>("localhost", 1883);

    if (cli->connect() == -1) {
        return -1;
    }

    pza::bps::ptr bps = std::make_shared<pza::bps>("default", "MY BPS 1");

    if (cli->register_device(bps) == -1)
        return -1;

    for (size_t i = 0; i < 1; i++) {
        auto bps_channel = bps->channel[i];

//        spdlog::info("Channel {}:", i);
//        bps_channel->ctrl.set_voltage(-7.3);
//        bps_channel->ctrl.set_current(1.0);
//        bps_channel->ctrl.set_enable(false);
//        spdlog::info("  Voltage: {}", bps_channel->voltmeter.get_measure());
//        spdlog::info("  Current: {}", bps_channel->ampermeter.get_measure());
//        spdlog::info("  Enabled: {}", bps_channel->ctrl.get_enable());
//        bps_channel->ctrl.set_enable(true);
//        bps_channel->ctrl.set_voltage(3.3);
//        bps_channel->ctrl.set_current(5.0);
//        spdlog::info("  Voltage: {}", bps_channel->voltmeter.get_measure());
//        spdlog::info("  Current: {}", bps_channel->ampermeter.get_measure());
//        spdlog::info("  Enabled: {}", bps_channel->ctrl.get_enable());

        //bps_channel->voltmeter.set_measure_polling_cycle(2);
        //bps_channel->ampermeter.set_measure_polling_cycle(2);
        bps_channel->ctrl.set_enable_polling_cycle(2);
    }

    spdlog::info("\n\nOK\n\n");

    return 0;
}
