#include "bps.hxx"

using namespace pza;

bps_channel::bps_channel(device *device, const std::string &base_name)
	: voltmeter(device, base_name + "vm"),
	ampermeter(device, base_name + "am"),
	ctrl(device, base_name + "ctrl")
{

}

bps::bps(const std::string &group, const std::string &name)
	: device(group, name)
{

}

int bps::_register_interfaces(const std::map<std::string, std::string> &map)
{
	int ret;
	
	ret = grouped_interface::register_interfaces<bps_channel>(this, "channel", map, channel);
	if (ret < 0)
		return ret;

	for (auto &chan : channel) {
		register_interface(chan->voltmeter);
		register_interface(chan->ampermeter);
		register_interface(chan->ctrl);
	}
	return 0;
}