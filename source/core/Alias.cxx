#include <core/Alias.hxx>

void Alias::show(void)
{
    std::cout << "Alias: " << id << std::endl;
    std::cout << "  url: " << url << std::endl;
    std::cout << "  port: " << port << std::endl;
    std::cout << "  interfaces: " << std::endl;
    for (auto &it : interfaces)
        std::cout << "    " << it.first << " -> " << it.second << std::endl;
}

bool Alias::hasInterface(const std::string &name)
{
    return interfaces.find(name) != interfaces.end();
}

int Alias::getInterfaceTopic(const std::string &name, std::string &topic)
{
    if (!hasInterface(name))
        return -1;
    topic = interfaces[name];
    return 0;
}

int Alias::getInterfaceNameFromTopic(const std::string &topic, std::string &name)
{
    auto it = std::find_if(interfaces.begin(), interfaces.end(),
                           [&](const auto &item)
                           {
                               return item.second == topic;
                           });
    if (it != interfaces.end()) {
        name = it->first;
        return 0;
    }
    return -1;
}