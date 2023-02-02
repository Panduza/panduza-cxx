#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <dirent.h>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <utils/Utils.hxx>
#include <core/Topic.hxx>
#include <core/Alias.hxx>

#ifndef API_VERSION
#define API_VERSION "Unknown version"
#endif

class Core
{
public:
    enum class LogLevel : int
    {
        Trace = spdlog::level::trace,
        Debug = spdlog::level::debug,
        Info = spdlog::level::info,
        Error = spdlog::level::err,
        Critical = spdlog::level::critical,
        None = spdlog::level::off
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
    static void removeAlias(const std::string &name);
    static void removeAliases(void);

private:
    static int _LoadInterfaces(const json &data, std::unordered_map<std::string, std::string> &map);
    static void _LoadAliasesFromJson(const std::string &payload);
    static void _addAlias(const Alias &alias);

    std::unordered_map<std::string, Alias> _aliases;
};