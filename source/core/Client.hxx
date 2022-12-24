#pragma once

#include <iostream>
#include <mosquittopp.h>
#include <spdlog/spdlog.h>
#include <core/Core.hxx>
#include <core/Interface.hxx>
#include <fnmatch.h>
#include <set>
#include <unistd.h>
#include <condition_variable>

#define DEFAULT_ADDR "localhost"
#define DEFAULT_PORT 1883

#define SCAN_TIMEOUT 5 // in seconds

class Client : public mosqpp::mosquittopp
{
public:
    explicit Client(const std::string &addr, int port);
    explicit Client(const std::string &alias);
    ~Client();

    int connect(void);
    int disconnect(void);
    int subscribe(const std::string &topic, const std::function<void(const std::string&, const std::string&)> &f);
    int publish(const std::string &topic, const void *payload, int len);
    int publish(const std::string &topic, const std::string &payload);
    bool connected(void) const { return _connected; }

    void on_connect(int rc) override;
    void on_disconnect(int rc) override;
    void on_message(const struct mosquitto_message *msg) override;
    void on_scan(const std::string &topic, const std::string &payload);

    void forEachInterface(const std::function<void(Interface&)> &f);
    void forEachListener(const std::function<void(const std::string&)> &f);
    void scan(int timeout = SCAN_TIMEOUT);
    void showScanResults(void);
    bool registerInterface(Interface &interface, const std::string &name);

private:
    std::string _addr;
    int _port;
    bool _connected = false;
    Core::Alias *_alias = nullptr;
    std::set<std::string> _scanResult;
    std::unordered_map<std::string, Interface*> _interfaces;
    std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>> _listeners;
    std::condition_variable _cv;
    int _scanCountPlatform;
    int _scanCountInterfaces;
};