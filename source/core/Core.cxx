#include <core/Core.hxx>

void Core::PrintVersion(void)
{
    std::cout << VERSION << std::endl;
}

void Core::SetLogLevel(const enum LogLevel &level)
{
    spdlog::set_level((spdlog::level::level_enum)level);
}

int Core::_LoadInterfaces(const json &data, std::unordered_map<std::string, std::string> &map)
{
    json tmp;

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
    _LoadAliasesFromJson<const std::string>(s);
}

void Core::LoadAliasesFromFile(const std::string &fileName)
{
    struct stat s;

    spdlog::trace("Reading alias from file {:s}", fileName);

    if (stat(fileName.c_str(), &s) || !S_ISREG(s.st_mode)) {
        spdlog::error("{:s} doesn't exist or is not a regular file.", fileName);
        return ;
    }

    std::ifstream file(fileName);
    if (file.is_open() == false) {
        spdlog::error("Could not open file {:s} : {:s}", fileName, strerror(errno));
        return ;
    }

    _LoadAliasesFromJson<std::ifstream>(file);
}

void Core::LoadAliasesFromDirectory(const std::string &dirName)
{
    struct stat s;

    if (stat(dirName.c_str(), &s) || !S_ISDIR(s.st_mode)) {
        spdlog::error("{:s} is not a directory.", dirName);
        return ;
    }

    for (const auto & entry : std::filesystem::directory_iterator(dirName))
        LoadAliasesFromFile(entry.path());
}

void Core::_addAlias(const Alias &alias)
{
    if (_aliases.count(alias.name) != 0)
        spdlog::warn("Alias {:s} already registered will be overwritten", alias.name);

    _aliases[alias.name] = alias;
}

Core::Alias *Core::findAlias(const std::string &name)
{
    Core::Alias *ret = nullptr;

    if (_aliases.count(name) != 0)
        ret = &_aliases[name];
    else
        spdlog::error("Alias named {:s} does not exist", name);
    return ret;
}