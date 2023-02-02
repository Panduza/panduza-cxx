#include <core/Interface.hxx>
#include <core/Client.hxx>

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
        return ;
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

void Interface::registerAttributes(void)
{
    std::string topic;

    for (auto const &atts : getAttributes()) {
        topic = _baseTopic + "/atts/" + atts->name();
        _atts.push_back(topic);
        _client.subscribe(topic, std::bind(&Attribute::onMessage, atts, std::placeholders::_1, std::placeholders::_2));
        atts->setCallback(std::bind(&Interface::dataFromAttribute, this, std::placeholders::_1));
    }
}

void Interface::unregisterAttributes(void)
{
    for (auto const &topic : _atts) {
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
    if (_client.isConnected() == true) {
        _client.unregisterInterface(*this);
    }
}