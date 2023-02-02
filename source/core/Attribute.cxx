#include <core/Attribute.hxx>

#include <iostream>

void Attribute::onMessage(const std::string &topic, const std::string &payload)
{
    json data;

    if (Utils::Json::ParseJson(payload, data) == -1)
        return ;
    onData(data[_name]);
}