#include <pza/core/client.hxx>
#include <pza/devices/bps.hxx>

int main(int argc, char** argv)
{
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <address> <port> <group> <BPS name>" << std::endl;
        return -1;
    }

    const char *address = argv[1];
    int port = std::stoi(argv[2]);
    const char *group = argv[3];
    const char *bps_name = argv[4];

    pza::core::set_log_level(pza::core::log_level::debug);

    pza::client::ptr cli = std::make_shared<pza::client>(address, port);

    if (cli->connect() == -1)
        return -1;

    pza::bps::ptr bps = std::make_shared<pza::bps>(group, bps_name);

    if (cli->register_device(bps) == -1)
        return -1;

    for (size_t i = 0; i < bps->get_num_channels(); i++) {
        auto bps_channel = bps->channel[i];
        spdlog::info("Channel {}:", i);
        bps_channel->ctrl.set_voltage(-7.3);
        bps_channel->ctrl.set_current(1.0);
        bps_channel->ctrl.set_enable(false);
        spdlog::info("  Voltage: {}", bps_channel->voltmeter.get_measure());
        spdlog::info("  Current: {}", bps_channel->ampermeter.get_measure());
        spdlog::info("  Enabled: {}", bps_channel->ctrl.get_enable());
    }

    return cli->disconnect();
}
