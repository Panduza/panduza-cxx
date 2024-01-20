#include <regex>

#include <pza/core/utils.hxx>

#include <spdlog/spdlog.h>

static const std::string regex_str = ":([a-zA-Z0-9]+)_([0-9]+):_([a-zA-Z0-9]+)";

namespace pza::utils
{
bool is_interface_grouped(const std::string &itf_name)
{
	std::regex pattern(regex_str);

	return std::regex_match(itf_name, pattern);
}

int get_grouped_interface_info(const std::string &itf_name, itf_group_info &info)
{
	std::regex pattern(regex_str);
	std::smatch matches;

	if (!std::regex_match(itf_name, matches, pattern)) {
		return -1;
	}

	info.group_name = matches[1].str();
	info.index = std::stoi(matches[2].str());
	info.interface_name = matches[3].str();

	return 0;
};

std::string format_interface_group(const std::string &group_name, unsigned int index, const std::string &interface_name)
{
	return ":" + group_name + "_" + std::to_string(index) + ":_" + interface_name;
};
};
