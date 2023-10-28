#include <pza/core/client.hxx>
#include <pza/core/core.hxx>
#include <pza/interfaces/ammeter.hxx>
#include <pza/interfaces/bpc.hxx>
#include <pza/interfaces/voltmeter.hxx>

static const std::string addr = "localhost";
static const unsigned int port = 1883;

int main()
{
	pza::core::set_log_level(pza::core::log_level::debug);

	pza::core::get_log_level();

	pza::client::s_ptr cli = std::make_shared<pza::client>(addr, port);

	if (cli->connect() < 0)
		return -1;

	if (cli->register_devices() < 0)
		return -1;

	for (auto &dev : cli->get_devices())
		spdlog::info("device: {}", dev->get_name());

	auto bps = cli->get_device("default", "Panduza_FakeBps");

	for (auto itf : bps->get_interfaces())
		spdlog::info("interface: {}", itf->get_name());

	auto vm = cli->get_interface<pza::itf::voltmeter>("default", "Panduza_FakeBps", "channel", 0, "vm");
	if (!vm) {
		spdlog::error("voltmeter not found");
		return -1;
	}

	auto ctrl = cli->get_interface<pza::itf::bpc>("default", "Panduza_FakeBps", "channel", 0, "ctrl");
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

	vm->register_measure_callback([&]() { spdlog::info("voltage: {}", vm->get_measure()); });

	am->register_measure_callback([&]() { spdlog::info("current: {}", am->get_measure()); });

	cli->disconnect();

	// while (true)
	//	;
	return 0;
}
