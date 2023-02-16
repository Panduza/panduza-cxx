#include <pza/core/Interface.hxx>
#include <pza/core/Client.hxx>

using namespace pza;

Interface::Interface(Client &client, const std::string &name)
    : _client(client)
{
    if (client.registerInterface(*this, name) == true) {
        _topic.setup(_baseTopic);
        _state = State::Init;
    }
    else {
        spdlog::error("Interface {:s} has not been scanned.", name);
        _state = State::Error;
    }
}

int Interface::init(void)
{
    if (_state == State::Init) {
        registerAttributes();
        _state = State::Run;
        return 0;
    }
    return -1;
}

void Interface::reconnect(void)
{
    if (_client.isConnected()) {
        _state = State::Init;
        init();
    }
}

void Interface::disconnect(void)
{
    if (_state == State::Unconnected)
        return ;
    unregisterAttributes();
    _state = State::Unconnected;
}

void Interface::registerAttributes(void)
{
    for (auto const &atts : getAttributes()) {
        std::string topic = _baseTopic + "/atts/" + atts->name();
        _attsTopic.push_back(topic);
        _client.subscribe(topic, std::bind(&Attribute::onMessage, atts, std::placeholders::_1, std::placeholders::_2));
        atts->setCallback(std::bind(&Interface::dataFromAttribute, this, std::placeholders::_1));
    }
}

void Interface::unregisterAttributes(void)
{
    for (auto const &topic : _attsTopic) {
        _client.unsubscribe(topic);
    }
}

void Interface::dataFromAttribute(const json &data)
{
    sendMessage(data.dump());
}

void Interface::sendMessage(const std::string &payload)
{
    _client.publish(_baseTopic + "/cmds/set", payload);
}

Interface::~Interface()
{
    if (_client.isSetup() == true) {
        _client.unregisterInterface(*this);
    }
}