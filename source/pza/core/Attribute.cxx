#include <pza/core/Attribute.hxx>

#include <iostream>

using namespace pza;

void Attribute::onMessage(const std::string &topic, const std::string &payload)
{
    json data;

    spdlog::trace("Attribute {:s} received message on topic {:s}, data : {:s}", _name, topic, payload);

    if (utils::json::ParseJson(payload, data) == -1)
        return ;
    onData(data[_name]);
}