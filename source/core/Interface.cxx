#include <core/Interface.hxx>

Interface::Interface(const Client &client, const Topic &topic)
    : _client(client),
    _topic(topic)
{

}

void Interface::on_message(const std::string &payload)
{

}

Interface::~Interface()
{

}