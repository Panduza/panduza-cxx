#include <core/Interface.hxx>
#include <core/Client.hxx>

Interface::Interface(Client &client, const std::string &name)
    : _client(client)
{
    if (client.registerInterface(*this, name) == true) {
        _topic.setup(_baseTopic);
    }
    else {
        spdlog::error("Interface {:s} has not been scanned.", name);
    }
}

void Interface::on_message(const std::string &payload)
{

}

Attribute &Interface::addAttribute(const std::string &name)
{
    //Attribute atts(name);

    //_attributes[name] = atts;
    //return _attributes[name];
}