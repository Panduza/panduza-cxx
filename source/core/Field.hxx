#pragma once

#include <string>

class Field
{
public:
    Field(const std::string &name);

    void set(void) {printf("SET\n");};
    void get(void) {printf("GET\n");};

private:
    std::string _name;
};