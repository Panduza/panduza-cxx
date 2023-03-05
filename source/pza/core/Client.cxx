#include <pza/core/Client.hxx>
#include <pza/core/Interface.hxx>

using namespace pza;

Client::Client(const std::string &addr, int port, const std::string &id)
{
    init(addr, port, id);
}

Client::Client(const std::string &alias)
{
    initAlias(alias);
}

void Client::initAlias(const std::string &alias)
{
    _alias = Core::Get().findAlias(alias);
    if (_alias) {
        std::string id_tmp = _alias->id;
        if (id_tmp == "")
            id_tmp = _generateRandomID();
        init(formatAddress(_alias->url, _alias->port), id_tmp);
    }
}

void Client::init(const std::string &addr, int port, const std::string &id)
{
    init(formatAddress(addr, port), (id == "") ? _generateRandomID() : id);
}

void Client::init(const std::string &url, const std::string &id)
{
    _url = url;
    _id = id;
    _pahoClient = std::make_unique<mqtt::async_client>(_url, id);
    _pahoClient->set_callback(_cb);
    _isSetup = true;
}

void Client::resetAlias(const std::string &alias)
{
    destroy();
    initAlias(alias);
}

void Client::reset(const std::string &addr, int port, const std::string &id)
{
    destroy();
    init(addr, port, id);
}

void Client::reset(const std::string &url, const std::string &id)
{
    destroy();
    init(url, id);
}

void Client::destroy(void)
{
    if (isConnected()) {
        disconnect();
    }
    else {
        unregisterInterfaces();
    }
    _pahoClient.reset();
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
    std::string ret;

    for (int i = 0; i < 16; i++) {
        ret += 'a' + dis(gen);
    }
    return ret;
}

void Client::abortScan(void)
{
    _scanAbort = true;
    _cv.notify_all();
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
        spdlog::error("Could not connect to {:s} : {:s}", _url, exc.what());
        return -1;
    }

    spdlog::debug("Connected to {:s}", _url.c_str());

    return scan(SCAN_TIMEOUT);
}

int Client::disconnect(void)
{
    if (_isSetup == false)
        return -1;

    abortScan();

    unconnectInterfaces();

    spdlog::debug("Disconnecting from {:s}", _url.c_str());
    try {
        _pahoClient->disconnect()->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not disconnect from {:s} : {:s}", _url, exc.what());
        return -1;
    }
    return 0;
}

int Client::reconnect(void)
{
    if (_isSetup == false)
        return -1;

    spdlog::debug("Attempting to reconnect to {:s}", _url.c_str());

    if (isConnected()) {
        spdlog::debug("Already connected to {:s}", _url.c_str());
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
        spdlog::error("Could not reconnect to {:s} : {:s}", _url, exc.what());
        return -1;
    }

    spdlog::debug("Reconnected to {:s}", _url.c_str());

    return scan(SCAN_TIMEOUT);
}

void Client::message_arrived(mqtt::const_message_ptr msg)
{
    std::string topic = msg->get_topic();
    std::string payload = msg->to_string();

    spdlog::trace("Received message on topic \"{:s}\" : \"{:s}\"", topic, payload);

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
        spdlog::error("Could not subscribe to {:s} : {:s}", _url, exc.what());
        return -1;
    }

    spdlog::trace("Subscribed to \"{:s}\"", topic);
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
        spdlog::error("Could not unsubscribe from {:s} : {:s}", _url, exc.what());
        return -1;
    }

    spdlog::trace("Unsubscribed from \"{:s}\"", topic);
    _listeners.erase(topic);
    return 0;
}

int Client::publish(const std::string &topic, const void *payload, int len)
{
    if (_isSetup == false)
        return -1;

    try {
        _pahoClient->publish(topic, payload, len, 0, false)->wait_for(std::chrono::seconds(CONN_TIMEOUT));
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("Could not publish to {:s} : {:s}", _url, exc.what());
        return -1;
    }
    return 0;
}

int Client::publish(const std::string &topic, const std::string &payload)
{
    return publish(topic, payload.c_str(), payload.length());
}

int Client::registerInterface(Interface &interface, const std::string &name)
{
    std::string &baseTopic = (std::string &)name;

    if (utils::string::StartsWith(name, "pza/") == false) {
        // is an alias
        pza::Core::ShowAliases();

        _alias->show();

        if (_alias && _alias->interfaces.count(name) > 0) {
            baseTopic = _alias->interfaces[name];
        }
        else {
            spdlog::error("Alias interface {:s} not found.", name);
            return -1;
        }
    }
    if (_scanResult.count(baseTopic)) {
        _interfaces[baseTopic] = &interface;
        interface.setBaseTopic(baseTopic);
        return 0;
    }
    return -1;
}

void Client::autoRegisterInterfaces(void)
{
    for (auto const &interface : _scanResult) {
        if (_interfaces.count(interface.first) == 0)
            Interface::CreateInterface(this, interface.first, interface.second);
    }
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
    std::string type;
    std::lock_guard<std::mutex> lock(_mtx);

    if (utils::json::ParseJson(payload, data) == -1)
        return ;

    if (utils::json::KeyExists(data, "info") && utils::json::ToString(data["info"], "type", type) == 0) {
        std::string name = topic.substr(0, topic.find("/atts/info"));
        
        if (type == "platform") {
            utils::json::ToInteger(data["info"], "interfaces", _scanCountPlatform);
            _scanResult[name] = Interface::StringToType(type);
            _scanCountInterfaces++;
        }
        else if (type != "unknown") {
            _scanResult[name] = Interface::StringToType(type);
            _scanCountInterfaces++;
        }
    }
    _cv.notify_all();
}

void Client::showScanResults(void)
{
    if (Core::LogLevel() > Core::LogLevel::Debug)
        return ;
    spdlog::debug("--- Scan results ---");
    spdlog::debug("Interface count: {:d}", _scanCountInterfaces);
    if (_scanCountInterfaces == 0)
        return ;
    spdlog::debug("List:");
    for (auto it = _scanResult.begin(); it != _scanResult.end(); it++) {
        spdlog::debug("  Interface {:02d}: {:s}", std::distance(_scanResult.begin(), it), it->first.c_str());
    }
    spdlog::debug("--------------------");
}

int Client::scan(int timeout)
{
    std::unique_lock<std::mutex> l(_mtx);

    _scanAbort = false;
    _scanCountInterfaces = 0;
    _scanCountPlatform = 0;
    _scanResult.clear();
    spdlog::debug("Start scanning...");
    subscribe("pza/+/+/+/+/info", std::bind(&Client::onScan, this, std::placeholders::_1, std::placeholders::_2));
    publish("pza", "*");
    auto const _scanComplete = [&](){return (_scanAbort || (_scanCountInterfaces != 0 && _scanCountInterfaces == _scanCountPlatform)); };

    if (_cv.wait_for(l, std::chrono::seconds(SCAN_TIMEOUT), _scanComplete) == false) {
        if (_scanCountPlatform == 0)
            spdlog::error("No Panduza platform seems to be running on the server.");
        else
            spdlog::warn("Scan didn't finish after {:d} seconds. Found {:d} interfaces but expected {:d}.", timeout, _scanCountInterfaces, _scanCountPlatform);
        return -1;
    }
    else if (_scanAbort) {
        spdlog::warn("Scan aborted.");
        return -1;
    }
    else {
        spdlog::debug("Scan successful! Found {:d} interfaces.", _scanCountInterfaces);
        showScanResults();
    }

    for (auto const &it : _scanResult) {
        if (_interfaces.count(it.first) > 0) {
            _interfaces.at(it.first)->reconnect();
        }
    }

    return 0;
}

Client::~Client()
{
    destroy();
}