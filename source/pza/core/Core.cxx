#include <pza/core/Core.hxx>
#include <pza/core/Interface.hxx>

using namespace pza;

void Core::PrintVersion(void)
{
    std::cout << LIBRARY_VERSION << std::endl;
}

void Core::SetLogLevel(const enum LogLevel &level)
{
    spdlog::set_level((spdlog::level::level_enum)level);
}

int Core::_LoadInterfaces(const json &data, std::unordered_map<std::string, std::string> &map)
{
    if (data.contains("interfaces") == false)
        return 0;

    const json &interfaces = data["interfaces"];

    if (interfaces.is_object() == false) {
        spdlog::error("Interfaces must be a JSON object");
        return -1;
    }

    for (json::const_iterator it = interfaces.cbegin(); it != interfaces.cend(); ++it) {
        const std::string &key = it.key();

        if (interfaces[key].is_string() == false) {
            spdlog::error("Interface alias {:s} must be a string", key);
            return -1;
        }
        map[key] = interfaces[key];
    }
    return 0;
}

void Core::LoadAliases(const std::string &s)
{
    _LoadAliasesFromJson(s);
}

void Core::LoadAliasesFromFile(const std::string &fileName)
{
    std::stringstream buf;

    spdlog::trace("Reading alias from file {:s}", fileName);

    if (std::filesystem::is_regular_file(fileName) == false) {
        spdlog::error("{:s} is not a regular file.", fileName);
        return ;
    }

    std::ifstream file(fileName);
    if (file.is_open() == false) {
        spdlog::error("Could not open file {:s} : {:s}", fileName, strerror(errno));
        return ;
    }

    buf << file.rdbuf();
    _LoadAliasesFromJson(buf.str());
}

void Core::_LoadAliasesFromJson(const std::string &payload)
{
    json data;
    
    if (utils::json::ParseJson(payload, data) == -1)
        return ;

    for (auto const &item : data.items()) {
        Alias alias;
        const std::string &key = item.key();
        const json &tmp = data[key];
        alias.id = key;
        
        if (utils::json::ToString(tmp, "url", alias.url) == -1
            || utils::json::ToInteger(tmp, "port", alias.port) == -1
            || _LoadInterfaces(tmp, alias.interfaces) == -1)
        {
            spdlog::error("Could not parse alias {:s}", item.key());
            continue;
        }
        Core::Get()._AddAlias(alias);
    }
}

void Core::LoadAliasesFromDirectory(const std::string &dirName)
{
    std::error_code ec;
    std::filesystem::directory_entry dirEntry{dirName, ec};

    if (ec) {
        spdlog::error("Could not access directory {:s} : {:s}", dirName, ec.message());
        return;
    }

    if (!dirEntry.is_directory(ec)) {
        spdlog::error("{:s} is not a directory.", dirName);
        return;
    }

    for (const auto &entry : std::filesystem::directory_iterator{dirName}) {
        if (entry.is_regular_file(ec)) {
            LoadAliasesFromFile(entry.path().string());
        }
    }

    if (ec)
        spdlog::error("Error while iterating through directory {:s} : {:s}", dirName, ec.message());
}

void Core::_AddAlias(const Alias &alias)
{
    if (Core::Get()._aliases.count(alias.id) != 0)
        spdlog::warn("Alias {:s} already registered will be overwritten", alias.id);

    Core::Get()._aliases[alias.id] = alias;
}

Alias *Core::findAlias(const std::string &name)
{
    Alias *ret = nullptr;

    if (Core::Get()._aliases.count(name) != 0)
        ret = &Core::Get()._aliases[name];
    else
        spdlog::error("Alias named {:s} does not exist", name);
    return ret;
}

void Core::RemoveAlias(const std::string &name)
{
    if (Core::Get()._aliases.count(name) != 0)
        Core::Get()._aliases.erase(name);
    else
        spdlog::error("Alias named {:s} does not exist", name);
}

void Core::RemoveAliases(void)
{
// use removeAlias instead in a safe for loop
    Core::Get()._aliases.clear();
}