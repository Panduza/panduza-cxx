#pragma once

#include <iostream>
#include <mqtt/client.h>
#include <spdlog/spdlog.h>
#include <core/Core.hxx>
#include <core/Alias.hxx>
#include <fnmatch.h>
#include <set>
#include <unistd.h>
#include <condition_variable>
#include <random>
#include <mutex>

#define DEFAULT_ADDR "localhost"
#define DEFAULT_PORT 1883

#define CONN_TIMEOUT 5 // in seconds
#define SCAN_TIMEOUT 5 // in seconds

class Interface;

class Client : public virtual mqtt::callback
{
    friend class Interface;

public:
    Client() = default;
    explicit Client(const std::string &addr, int port, const std::string &id = "");
    explicit Client(const std::string &alias);
    ~Client();

    int connect(void);
    int disconnect(void);
    int reconnect(void);
    void init(const std::string &alias);
    void init(const std::string &addr, const std::string &id);
    void reset(const std::string &alias);
    void reset(const std::string &addr, const std::string &id);
    void destroy(void);
    bool isSetup(void) const
    {
        return _isSetup;
    }

    std::string formatAddress(const std::string &addr, int port);
    const std::string &address(void) const
    {
        return _addr;
    }
    const std::string &id(void) const
    {
        return _id;
    }

    bool isConnected(void) const
    {
        int ret = false;

        if (_isSetup)
            ret = _pahoClient->is_connected();
        return ret;
    }

    int scan(int timeout = SCAN_TIMEOUT);

private:

    void connection_lost(const std::string &cause) override
    {
        spdlog::warn("Connection lost");
        if (!cause.empty())
            spdlog::warn("\tcause: {:s}", cause);
        unconnectInterfaces();
    }
    
    void onScan(const std::string &topic, const std::string &payload);
    void showScanResults(void);
    void message_arrived(mqtt::const_message_ptr msg) override;
    int subscribe(const std::string &topic, const std::function<void(const std::string &, const std::string &)> &f);
    int unsubscribe(const std::string &topic);
    int publish(const std::string &topic, const void *payload, int len);
    int publish(const std::string &topic, const std::string &payload);
    void unconnectInterfaces(void);
    bool registerInterface(Interface &interface, const std::string &name);
    void unregisterInterface(Interface &interface);
    void unregisterInterfaces(void);
    std::string _generateRandomID(void);

    bool _isSetup = false;
    std::unique_ptr<mqtt::async_client> _pahoClient;
    callback _cb;
    std::string _addr;
    std::string _id;
    Alias *_alias = nullptr;
    std::set<std::string> _scanResult;
    std::unordered_map<std::string, Interface *> _interfaces;
    std::unordered_map<std::string, std::function<void(const std::string &, const std::string &)>> _listeners;
    std::condition_variable _cv;
    std::mutex _mtx;
    int _scanCountPlatform = 0;
    int _scanCountInterfaces = 0;
};