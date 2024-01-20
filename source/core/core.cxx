#include <pza/core/core.hxx>

void pza::core::set_log_level(pza::core::log_level level)
{
	spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}

pza::core::log_level pza::core::get_log_level()
{
	return static_cast<pza::core::log_level>(spdlog::get_level());
}
