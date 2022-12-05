#pragma once

#include <spdlog/spdlog.h>
#include <pza/version.hxx>

namespace pza
{
    class core
    {
    public:
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

        core() = delete;
        ~core() = delete;

        static void set_log_level(log_level level);
        static log_level get_log_level(void);
        static std::string get_version(void);
    };
};