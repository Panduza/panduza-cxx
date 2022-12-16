#pragma once

#include <iostream>
#include <mosquittopp.h>
#include <spdlog/spdlog.h>
#include <core/Core.hxx>
#include <core/Interface.hxx>
#include <fnmatch.h>

#define DEFAULT_ADDR "localhost"
#define DEFAULT_PORT 1883

class Client : public mosqpp::mosquittopp
{
public:
    explicit Client(const std::string &addr, int port);
    explicit Client(const std::string &alias);
    ~Client();

    int connect(void);
    int disconnect(void);
    int subscribe(const std::string &topic, const std::function<void(const std::string &payload)> &f);
    int publish(const std::string &topic, const void *payload, int len);
    int publish(const std::string &topic, const std::string &payload);
    bool connected(void) const { return _connected; }
    void on_connect(int rc) override;
    void on_disconnect(int rc) override;
    void on_message(const struct mosquitto_message *msg) override;

    void forEachInterface(const std::function<void(Interface &interface)> &f);
    void forEachListener(const std::function<void(const std::string&)> &f);
    void on_info(const std::string &payload);
    const std::vector<std::string> &scanInterfaces(void);
    void discover(void);
    bool interfaceIsRegistered(const std::string &topic);

private:
    std::string _addr;
    int _port;
    bool _connected = false;
    const Core::Alias *_alias = nullptr;
    std::unordered_map<std::string, Interface> _interfaces;
    std::unordered_map<std::string, std::function<void(const std::string&)>> _listeners;
};