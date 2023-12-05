#include <pza/core/core.hxx>

using namespace pza;

void core::set_log_level(log_level level)
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}

core::log_level core::get_log_level(void)
{
    return static_cast<core::log_level>(spdlog::get_level());
}

std::string core::get_version(void)
{
    return PZACXX_VERSION;
}