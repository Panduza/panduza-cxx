#pragma once

#include <pza/core/device.hxx>
#include <pza/core/grouped_interface.hxx>

#include <pza/interfaces/meter.hxx>
#include <pza/interfaces/bps_chan_ctrl.hxx>

#include <pza/utils/string.hxx>

namespace pza
{
	class bps_channel
	{
    public:
		using ptr = std::shared_ptr<bps_channel>;

		friend class bps;

        bps_channel(device *device, const std::string &base_name);

        meter voltmeter;
        meter ampermeter;
        bps_chan_ctrl ctrl;
    };

	class bps : public device
	{
	public:
		using ptr = std::shared_ptr<bps>;

		explicit bps(const std::string &group, const std::string &name);

		const std::string &get_family() override { return _family; };
		size_t get_num_channels() { return channel.size(); }
		std::vector<bps_channel::ptr> channel;

	private:
        int _register_interfaces(const std::map<std::string, std::string> &map) override;

        std::string _family = "bps";
	};
};
