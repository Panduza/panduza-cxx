#pragma once

#include <string>

class Attribute
{
public:
    Attribute(const std::string &name);

private:
    std::string _name;
};