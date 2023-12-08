#include <pza/core/core.hxx>

namespace pza::core
{
    void set_log_level(log_level level)
    {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
    }

    log_level get_log_level(void)
    {
        return static_cast<log_level>(spdlog::get_level());
    }
}
