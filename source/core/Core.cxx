#include <core/Core.hxx>
#include <core/Interface.hxx>

void Core::PrintVersion(void)
{
    std::cout << API_VERSION << std::endl;
}

void Core::SetLogLevel(const enum LogLevel &level)
{
    spdlog::set_level((spdlog::level::level_enum)level);
}

int Core::_LoadInterfaces(const json &data, std::unordered_map<std::string, std::string> &map)
{
    json tmp;

    if (data.contains("interfaces") == false)
        return 0;

    if (Utils::Json::ToObject(data, "interfaces", tmp) == -1)
        return -1;

    for (auto const &item : tmp.items()) {
        const std::string &key = item.key();

        if (tmp[key].is_string() == false) {
            spdlog::error("Interface alias {:s} must be a string", key);
            return -1;
        }
        map[key] = tmp[key];
    }
    return 0;
}

void Core::LoadAliases(const std::string &s)
{
    _LoadAliasesFromJson(s);
}

void Core::LoadAliasesFromFile(const std::string &fileName)
{
    struct stat s;

    spdlog::trace("Reading alias from file {:s}", fileName);

    if (stat(fileName.c_str(), &s) == -1) {
        spdlog::error("Could not stat file {:s} : {:s}", fileName, strerror(errno));
        return ;
    }
    if (!S_ISREG(s.st_mode)) {
        spdlog::error("{:s} is not a regular file.", fileName);
        return ;
    }

    std::ifstream file(fileName);
    if (file.is_open() == false) {
        spdlog::error("Could not open file {:s} : {:s}", fileName, strerror(errno));
        return ;
    }

    std::stringstream buf;

    buf << file.rdbuf();

    _LoadAliasesFromJson(buf.str());
}

void Core::_LoadAliasesFromJson(const std::string &payload)
{
    json data;
    
    if (Utils::Json::ParseJson(payload, data) == -1)
        return ;

    for (auto const &item : data.items()) {
        Alias alias;
        const std::string &key = item.key();
        const json &tmp = data[key];
        alias.id = key;
        
        if (Utils::Json::ToString(tmp, "url", alias.url) == -1
            || Utils::Json::ToInteger(tmp, "port", alias.port) == -1
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
    struct stat s;
    DIR* dir;
    struct dirent* entry;

    if (stat(dirName.c_str(), &s) == -1) {
        spdlog::error("Could not stat directory {:s} : {:s}", dirName, strerror(errno));
        return ;
    }

    if (!S_ISDIR(s.st_mode)) {
        spdlog::error("{:s} is not a directory.", dirName);
        return ;
    }

    dir = opendir(dirName.c_str());
    if (dir == nullptr) {
        spdlog::error("Could not open directory {:s} : {:s}", dirName, strerror(errno));
        return ;
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        LoadAliasesFromFile(dirName + "/" + entry->d_name);
    }
    closedir(dir);
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
    if (Core::Get()._aliases.count(name) != 0) {
        Core::Get()._aliases.erase(name);
    }
    else
        spdlog::error("Alias named {:s} does not exist", name);
}

void Core::RemoveAliases(void)
{
// use removeAlias instead in a safe for loop
    Core::Get()._aliases.clear();
}