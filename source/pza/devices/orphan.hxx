#pragma once

#include <pza/core/device.hxx>
#include <pza/core/grouped_interface.hxx>


#include <pza/utils/string.hxx>

namespace pza
{

	class orphan : public device
	{
	public:
		using ptr = std::shared_ptr<orphan>;

		explicit orphan(const std::string &group, const std::string &name);

		const std::string &get_family() override { return _family; };
		// size_t get_num_channels() { return channel.size(); }
		// std::vector<bps_channel::ptr> channel;

	private:
        int _register_interfaces(const std::map<std::string, std::string> &map) override;

        std::string _family = "orphan";
	};
};
