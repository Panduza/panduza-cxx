#pragma once

#include <iostream>
#include <unordered_map>

#include <core/Topic.hxx>
#include <core/Attribute.hxx>

class Client;

class Interface
{

    friend class Client;

protected:
    Interface(Client &client, const std::string &name);

    const std::string &name(void) { return _topic.interface(); }

    Attribute &addAttribute(const std::string &name);

private:
    enum class eState {
        Init,
        Run,
        Error
    };
    
    void on_message(const std::string &payload);
    void setBaseTopic(const std::string &name) { _baseTopic = name; }
    
    std::unordered_map<std::string, Attribute> _attributes;
    std::string _baseTopic;
    Client &_client;
    Topic _topic;
    eState _state;
};