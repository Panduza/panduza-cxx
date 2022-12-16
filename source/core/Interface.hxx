#pragma once

#include <iostream>

#include <core/Topic.hxx>

class Client;

class Interface
{
public:
    Interface(const Client &client, const Topic &topic);
    ~Interface();

    const std::string &name(void) { return _topic.interface; }

    void setThreat(bool state) { _threatened = state;}
    bool isThreatened(void) const { return _threatened;}

    void on_message(const std::string &payload);

private:
    const Client &_client;
    const Topic &_topic;
    bool _threatened;
};