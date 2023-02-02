#pragma once

#include <string>
#include <sstream>
#include <array>

class Topic
{
public:
    Topic() = default;

    const std::string &machine(void) const {return _list[Id::Machine];}
    const std::string &group(void) const {return _list[Id::Group];}
    const std::string &interface(void) const {return _list[Id::Interface];}

    void setup(const std::string &name);

private:
    enum Id {
        Machine,
        Group,
        Interface,
        Last // Keep Last
    };

    std::array<std::string, Id::Last> _list;
};