#include <core/Client.hxx>
#include <core/Interface.hxx>

Client::Client(const std::string &addr, int port, const std::string &id)
{
    if (_id == "")
        _id = _generateRandomID();

    init(formatAddress(addr, port), _id);
}

Client::Client(const std::string &alias)
{
    init(alias);
}

void Client::init(const std::string &alias)
{
    std::string id;

    _alias = Core::Get().findAlias(alias);
    if (_alias) {
        id = _alias->id;
        if (id == "")
            id = _generateRandomID();
        init(formatAddress(_alias->url, _alias->port), id);
    }
}

void Client::init(const std::string &addr, const std::string &id)
{
    _addr = addr;
    _id = id;
    _pahoClient = std::make_unique<mqtt::async_client>(addr, id);
    _pahoClient->set_callback(_cb);
    _isSetup = true;
}

void Client::reset(const std::string &alias)
{
    destroy();
    init(alias);
}

void Client::reset(const std::string &addr, const std::string &id)
{
    destroy();
    init(addr, id);
}

void Client::destroy(void)
{
    if (isConnected()) {
        disconnect();
    }
    else {
        unregisterInterfaces();
    }
    _listeners.clear();
    _isSetup = false;
}

std::string Client::formatAddress(const std::string &addr, int port)
{
    return "tcp://" + addr + ":" + std::to_string(port);
}

std::string Client::_generateRandomID(void)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 25);
    std::string id;

    for (int i = 0; i < 16; i++) {
        id += 'a' + dis(gen);
    }
    return id;
}

int Client::connect(void)
{
    mqtt::connect_options opts;

    if (isSetup() == false)
        return -1;
    
    opts.set_keep_alive_interval(20);
    _pahoClient->set_callback(*this);

    try {
        _pahoClient->connect(opts)->wait_for(std::chrono::seconds(CONN_TIMEOUT));
        if (!_pahoClient->is_connected()) {
            spdlog::error("Connection timed out");
            return -1;
        }
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not connect to {:s} : {:s}", _addr, exc.what());
        return -1;
    }

    spdlog::debug("Connected to {:s}", _addr.c_str());

    for (auto &it : _interfaces) {
        it.second->reconnect();
    }

    return scan(SCAN_TIMEOUT);
}

int Client::disconnect(void)
{
    if (_isSetup == false)
        return -1;

    unconnectInterfaces();

    spdlog::debug("Disconnecting from {:s}", _addr.c_str());
    try {
        _pahoClient->disconnect()->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not disconnect from {:s} : {:s}", _addr, exc.what());
        return -1;
    }
    return 0;
}

int Client::reconnect(void)
{
    if (_isSetup == false)
        return -1;

    spdlog::debug("Attempting to reconnect to {:s}", _addr.c_str());

    if (isConnected()) {
        spdlog::debug("Already connected to {:s}", _addr.c_str());
        disconnect();
    }

    try {
        _pahoClient->reconnect()->wait_for(std::chrono::seconds(CONN_TIMEOUT));
        if (!_pahoClient->is_connected()) {
            spdlog::error("Connection timed out");
            return -1;
        }
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not reconnect to {:s} : {:s}", _addr, exc.what());
        return -1;
    }

    spdlog::debug("Reconnected to {:s}", _addr.c_str());

    for (auto &it : _interfaces) {
        it.second->reconnect();
    }

    return scan(SCAN_TIMEOUT);
}

void Client::message_arrived(mqtt::const_message_ptr msg)
{
    std::string topic = msg->get_topic();
    std::string payload = msg->to_string();

    if (_listeners.count(topic) > 0) {
        _listeners[topic](topic, payload); // execute callback at index
    } else {
        for (auto &[key, value] : _listeners) {
            if (fnmatch(key.c_str(), topic.c_str(), 0) == 0) {
                value(topic, payload); // execute callback for matching key
                break;
            }
        }
    }
}

int Client::subscribe(const std::string &topic, const std::function<void(const std::string &topic, const std::string &payload)> &f)
{
    std::string s = topic;

    if (_isSetup == false)
        return -1;

    try {
        _pahoClient->subscribe(topic, 0)->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not subscribe to {:s} : {:s}", _addr, exc.what());
        return -1;
    }

    spdlog::trace("Registering listener for \"{:s}\"", topic);
    std::replace(s.begin(), s.end(), '+', '*');
    std::replace(s.begin(), s.end(), '#', '*');
    _listeners[s] = f;
    
    return 0;
}

int Client::unsubscribe(const std::string &topic)
{
    if (_isSetup == false)
        return -1;

    try {
        _pahoClient->unsubscribe(topic)->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not unsubscribe from {:s} : {:s}", _addr, exc.what());
        return -1;
    }

    spdlog::trace("Unregistering listener for \"{:s}\"", topic);
    _listeners.erase(topic);
    return 0;
}

int Client::publish(const std::string &topic, const void *payload, int len)
{
    if (_isSetup == false)
        return -1;

    try {
        _pahoClient->publish(topic, (const char *)payload, len, 0, false)->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not publish to {:s} : {:s}", _addr, exc.what());
        return -1;
    }
    return 0;
}

int Client::publish(const std::string &topic, const std::string &payload)
{
    return publish(topic, payload.c_str(), payload.length());
}

bool Client::registerInterface(Interface &interface, const std::string &name)
{
    bool ret = false;
    std::string &baseTopic = (std::string &)name;

    if (Utils::String::StartsWith(name, "pza/") == false) {
        // is an alias
        if (_alias && _alias->interfaces.count(name) > 0) {
            baseTopic = _alias->interfaces[name];
        }
        else {
            spdlog::error("Alias {:s} not found.", name);
            return false;
        }
    }
    if (_scanResult.count(baseTopic)) {
        _interfaces[baseTopic] = &interface;
        interface.setBaseTopic(baseTopic);
        ret = true;
    }
    return ret;
}

void Client::unconnectInterfaces(void)
{
    for (auto &it : _interfaces) {
        it.second->disconnect();
    }
}

void Client::unregisterInterface(Interface &interface)
{
    interface.disconnect();
    _interfaces.erase(interface._baseTopic);
}

void Client::unregisterInterfaces(void)
{
    for (auto &it : _interfaces) {
        it.second->disconnect();
    }
    _interfaces.clear();
}

void Client::onScan(const std::string &topic, const std::string &payload)
{
    json data;
    std::string tmp;
    std::unique_lock<std::mutex> lock(_mtx);

    if (Utils::Json::ParseJson(payload, data) == -1)
        return ;

    if (Utils::Json::KeyExists(data, "info") && Utils::Json::ToString(data["info"], "type", tmp) == 0) {
        if (tmp == "platform")
            Utils::Json::ToInteger(data["info"], "interfaces", _scanCountPlatform);
        _scanResult.emplace(topic.substr(0, topic.find("/atts/info")));
        _scanCountInterfaces++;
    }
    _cv.notify_one();
}

void Client::showScanResults(void)
{
    spdlog::debug("--- Scan results ---");
    spdlog::debug("Interface count: {:d}", _scanCountInterfaces);
    if (_scanCountInterfaces == 0)
        return ;
    spdlog::debug("List:");
    for (auto it = _scanResult.begin(); it != _scanResult.end(); it++) {
        spdlog::debug("  Interface {:02d}: {:s}", std::distance(_scanResult.begin(), it), *it);
    }
    spdlog::debug("--------------------");
}

int Client::scan(int timeout)
{
    int ret = 0;
    std::unique_lock<std::mutex> l(_mtx);

    using namespace std::chrono_literals;

    _scanCountInterfaces = 0;
    _scanCountPlatform = 0;
    _scanResult.clear();
    spdlog::debug("Start scanning...");
    subscribe("pza/+/+/+/+/info", std::bind(&Client::onScan, this, std::placeholders::_1, std::placeholders::_2));
    publish("pza", "*");
    auto const _scanComplete = [&](){ return (_scanCountInterfaces != 0 && _scanCountInterfaces == _scanCountPlatform); };
    if (_cv.wait_for(l, std::chrono::seconds(timeout), _scanComplete) == false) {
        if (_scanCountPlatform == 0)
            spdlog::error("No Panduza platform seems to be running on the server.");
        else {
            spdlog::warn("Scan didn't finish after {:d} seconds. Found {:d} interfaces but expected {:d}.", timeout, _scanCountInterfaces, _scanCountPlatform);
        }
        ret = -1;
    }
    else {
        spdlog::debug("Scan successful! Found {:d} interfaces.", _scanCountInterfaces);
    }
    showScanResults();
    return ret;
}

Client::~Client()
{
    if (isConnected())
        disconnect();

    unregisterInterfaces();
}