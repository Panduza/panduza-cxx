#include <core/Attribute.hxx>

#include <iostream>

void Attribute::onMessage(const std::string &topic, const std::string &payload)
{
    json data;

    spdlog::trace("Attribute {:s} received message on topic {:s}, data : {:s}", _name, topic, payload);

    if (Utils::Json::ParseJson(payload, data) == -1)
        return ;
    onData(data[_name]);
}