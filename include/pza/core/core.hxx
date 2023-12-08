#pragma once

#include <spdlog/spdlog.h>

namespace pza
{
namespace core
{
    enum class log_level : int
    {
        trace = spdlog::level::trace,
        debug = spdlog::level::debug,
        info = spdlog::level::info,
        warn = spdlog::level::warn,
        err = spdlog::level::err,
        critical = spdlog::level::critical,
        off = spdlog::level::off
    };

    void set_log_level(pza::core::log_level level);
    pza::core::log_level get_log_level(void);
    
    std::string get_version();
    std::string get_version_githash();
    unsigned int get_version_major();
    unsigned int get_version_minor();
    unsigned int get_version_patch();
};
};
