#pragma once

#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>

namespace pza
{
    class Alias
    {
    public:
        std::string id;
        std::string url;
        int port;
        std::unordered_map<std::string, std::string> interfaces;

        void show(void) const;
        bool hasInterface(const std::string &name);
        int getInterfaceTopic(const std::string &name, std::string &topic);
        int getInterfaceNameFromTopic(const std::string &topic, std::string &name);
    };
};