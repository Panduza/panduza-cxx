#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <utils/Utils.hxx>
#include <core/Topic.hxx>

#ifndef VERSION
#define VERSION "Unknown version"
#endif


class PzaException : std::exception
{
public:
    PzaException(const std::string &msg)
        : message(msg)
    {
        
    }

    const std::string &what() { return message; }

private:
    std::string message;
};

class Core
{
public:
    struct Alias
    {
        std::string name;
        std::string url;
        int port;
        std::unordered_map<std::string, std::string> interfaces;
    };

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
    static void LoadAliasesFromFile(const std::string &file);
    static void LoadAliasesFromDirectory(const std::string &dirName);
    const std::unordered_map<std::string, Alias> &aliases(void) const { return _aliases; }
    static void ShowAliases()
    {
        spdlog::info("--------- Aliases ---------");
        for (auto const &item : Core::Get().aliases()) {
            spdlog::info("Name: {:s}", item.second.name);
            spdlog::info("Url : {:s}", item.second.url);
            spdlog::info("Port: {:d}", item.second.port);
            spdlog::info("Interfaces:");
            for (auto const &itf : item.second.interfaces) {
                spdlog::info("  {:s} : {:s}", itf.first, itf.second);
            }
        }
        spdlog::info("---------------------------");
    }
    Alias *findAlias(const std::string &name);

private:
    static int _LoadInterfaces(const json &data, std::unordered_map<std::string, std::string> &map);
    static void _LoadAliasesFromJson(const json &data);

    template <typename T>
    static void _LoadAliasesFromJson(T &source)
    {
        json data;

        try {
            data = json::parse(source);
        }
        catch (json::parse_error &error)
        {
            spdlog::error("Could not parse JSON : {:s}", error.what());
            return ;
        }

        for (auto const &item : data.items()) {
            Alias alias;
            const std::string &key = item.key();
            const json &tmp = data[key];

            alias.name = key;
            
            if (Utils::Json::ToString(tmp, "url", alias.url) == -1
                || Utils::Json::ToInteger(tmp, "port", alias.port) == -1
                || Utils::Json::KeyExists(tmp, "interfaces") == false
                || _LoadInterfaces(tmp, alias.interfaces) == -1)
            {
                spdlog::error("Could not parse alias {:s}", item.key());
                continue;
            }
            Core::Get()._addAlias(alias);
        }
    }
    void _addAlias(const Alias &alias);

    std::unordered_map<std::string, Alias> _aliases;
};