#include <pza/core/Interface.hxx>
#include <pza/core/Client.hxx>

#include <pza/interfaces/Psu.hxx>

using namespace pza;

Interface::Interface(const std::string &name)
    : _tmpName(name),
    _state(State::Orphan)
{
    
}

void Interface::bindToClient(Client *client)
{
    if (!client) {
        spdlog::error("Client is null.");
        return ;
    }

    if (_state != State::Orphan) {
        spdlog::error("Interface {:s} is already bound to client {:s}.", _client->id());
        return ;
    }
    
    _client = client;
    if (client->registerInterface(*this, _tmpName) == 0) {
        _topic.setup(_baseTopic);
        registerAttributes();
        subscribe();
        (_client->isConnected()) ? _state = State::Running : _state = State::Unconnected;
    }
    else {
        spdlog::error("Interface {:s} has not been scanned.", _tmpName);
        _state = State::Unconnected;
    }
}

void Interface::unbind(void)
{
    if (_state == State::Orphan)
        return ;
    _client->unregisterInterface(*this);
    _state = State::Orphan;
}

void Interface::reconnect(void)
{
    if (_state == State::Unconnected && _client->isConnected()) {
        spdlog::trace("Interface {:s} reconnected.", name());
        _state = State::Running;
        subscribe();
    }
}

void Interface::disconnect(void)
{
    if (_state == State::Unconnected)
        return ;
    spdlog::trace("Interface {:s} disconnected.", name());
    _state = State::Unconnected;
}

void Interface::subscribe(void)
{
    for (auto const &atts : getAttributes()) {
        _client->subscribe(atts->topic(), std::bind(&Attribute::onMessage, atts, std::placeholders::_1, std::placeholders::_2));
    }
}

void Interface::registerAttributes(void)
{
    _attsTopic.clear();
    for (auto const &atts : getAttributes()) {
        std::string topic = _baseTopic + "/atts/" + atts->name();
        atts->setTopic(topic);
        _attsTopic.push_back(topic);
        atts->setCallback(std::bind(&Interface::dataFromAttribute, this, std::placeholders::_1));
    }
}

void Interface::unsubscribe(void)
{
    for (auto const &topic : _attsTopic) {
        _client->unsubscribe(topic);
    }
}

void Interface::CreateInterface(Client *client, const std::string &name, Interface::Type type)
{
    switch(type) {
        case Interface::Type::Psu:
        {
            auto interface = new Psu(name);
            interface->bindToClient(client);
            break;
        }
        case Interface::Type::Platform:
            break;
        default:
            spdlog::error("Unknown interface type for {:s}", name);
            break;
    }
}

void Interface::dataFromAttribute(const json &data)
{
    sendMessage(data.dump());
}

void Interface::sendMessage(const std::string &payload)
{
    if (_state != State::Running)
        return ;
    _client->publish(_baseTopic + "/cmds/set", payload);
}

Interface::~Interface()
{
    if (_client && _client->isConnected()) {
        _client->unregisterInterface(*this);
    }
}