#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <system_error>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <pza/utils/Utils.hxx>
#include <pza/core/Topic.hxx>
#include <pza/core/Alias.hxx>
#include <pza/version.hxx>

using namespace nlohmann;

namespace pza
{
    class Core
    {
    public:
        enum class LogLevel : int
        {
            Trace = spdlog::level::trace,
            Debug = spdlog::level::debug,
            Info = spdlog::level::info,
            Warn = spdlog::level::warn,
            Error = spdlog::level::err,
            Critical = spdlog::level::critical,
            Off = spdlog::level::off
        };

        Core() = default;
        Core(const Core &) = delete;
        Core &operator=(const Core &) = delete;

        static Core &Get(void)
        {
            static Core instance;

            return instance;
        }

        static void PrintVersion(void);
        static void SetLogLevel(const enum LogLevel &);
        static enum LogLevel LogLevel(void)
        {
            return (enum LogLevel)spdlog::get_level();
        }
        static void LoadAliases(const std::string &s);
        static void LoadAliasesFromFile(const std::string &fileName);
        static void LoadAliasesFromDirectory(const std::string &dirName);
        static size_t AliasesCount(void)
        {
            return Core::Get()._aliases.size();
        }

        static void ShowAliases()
        {
            spdlog::debug("--------- Aliases ---------");
            for (auto &item : Core::Get()._aliases)
                item.second.show();
            spdlog::debug("---------------------------");
        }
        static Alias *findAlias(const std::string &name);
        static void RemoveAlias(const std::string &name);
        static void RemoveAliases(void);

    private:
        static int _LoadInterfaces(const json &data, std::unordered_map<std::string, std::string> &map);
        static void _LoadAliasesFromJson(const std::string &payload);
        static void _AddAlias(const Alias &alias);

        std::unordered_map<std::string, Alias> _aliases;
    };
};