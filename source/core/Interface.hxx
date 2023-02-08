#pragma once

#include <iostream>
#include <unordered_map>

#include <core/Topic.hxx>
#include <core/Attribute.hxx>

class Client;

class Interface
{
    enum class State
    {
        Unconnected,
        Init,
        Run,
        Error
    };

    friend class Client;

public:
    bool isRunning(void) const
    {
        return (_state == State::Run);
    }

    virtual int init(void);

protected:
    explicit Interface(Client &client, const std::string &name);
    ~Interface();

    const std::string &name(void) const
    {
        return _topic.interface();
    }

    virtual const std::vector<Attribute *> &getAttributes() const = 0;

private:

    void setState(State state)
    {
        _state = state;
    }

    void setBaseTopic(const std::string &name)
    {
        _baseTopic = name;
    }

    void dataFromAttribute(const json &data);
    void registerAttributes(void);
    void unregisterAttributes(void);
    void sendMessage(const std::string &payload);
    void reconnect(void);
    void disconnect(void);

    State _state;
    Topic _topic;
    Client &_client;
    std::string _baseTopic;
    std::vector<std::string> _atts;
};