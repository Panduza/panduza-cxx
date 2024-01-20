#pragma once

#include <optional>
#include <string>

namespace pza::utils
{

struct itf_group_info {
	std::string group_name;
	unsigned int index = 0;
	std::string interface_name;
};

bool is_interface_grouped(const std::string &itf_name);

int get_grouped_interface_info(const std::string &itf_name, itf_group_info &info);

std::string format_interface_group(const std::string &group_name, unsigned int index,
				   const std::string &interface_name);
};
