#include <core/Client.hxx>

Client::Client(const std::string &addr, int port)
    : _addr(addr),
    _port(port),
    _connected(false)
{
    mosqpp::lib_init();
    mosquittopp::loop_start();
}

Client::Client(const std::string &aliasName)
{
    _alias = Core::Get().findAlias(aliasName);
    if (_alias) {
        _addr = _alias->url;
        _port = _alias->port;
    }
    else {
        spdlog::warn("Setting client back to default values");
        _addr = DEFAULT_ADDR;
        _port = DEFAULT_PORT;
    }

    mosquittopp::loop_start();
    mosqpp::lib_init();
}


void Client::on_connect(int rc)
{
    switch (rc) {
        case MOSQ_ERR_SUCCESS:
            _connected = true;
            spdlog::info("Connected to {:s} on port {:d}", _addr.c_str(), _port);
            break;
        default:
            _connected = false;
            spdlog::warn("Something went wrong when trying to connect");
            break;
    }
}

void Client::on_disconnect(int rc)
{
    switch (rc) {
        case MOSQ_ERR_SUCCESS:
            spdlog::info("Disconnected from {:s}", _addr.c_str());
            break;
        default:
            spdlog::warn("Unexpected disconnect");
            break;
    }
    _connected = false;
}

int Client::connect(void)
{
    int ret;

    spdlog::info("Attempting to connect to {:s} on port {:d}", _addr.c_str(), _port);
    ret = mosqpp::mosquittopp::connect(_addr.c_str(), _port);
    switch (ret) {
        case MOSQ_ERR_SUCCESS:
            break;
        default:
            spdlog::error("Could not connect to {:s} : {:s}", _addr, mosquitto_strerror(ret));
            // We don't call Client::disconnect here because this is not a real disconnect.
            // It is just used to clean mosquitto loop so that we don't have to force the loop stop in the destructor
            mosqpp::mosquittopp::disconnect();
            break;
    }
    return (ret == 0) ? 0 : -1;
}

int Client::disconnect(void)
{
    spdlog::info("Disconnecting from {:s} on port {:d}", _addr.c_str(), _port);
    return mosqpp::mosquittopp::disconnect();
}

int Client::subscribe(const std::string &topic, const std::function<void(const std::string &lol, const std::string &payload)> &f)
{
    int ret;
    std::string s = topic;
    
    ret = mosqpp::mosquittopp::subscribe(NULL, topic.c_str());
    if (ret == MOSQ_ERR_SUCCESS) {
        spdlog::trace("Registering listener for \"{:s}\"", topic);
        std::transform(s.begin(), s.end(), s.begin(), [](char c) {
            switch (c) {
                case '+':
                case '#':
                    return '*';
                default:
                    return c;
            }
        });
        _listeners[s] = f;
    }
    else
        spdlog::warn("Could not subscribe to topic \"{:s}\"", topic.c_str());
    return ret;
}

int Client::publish(const std::string &topic, const void *payload, int len)
{
    int ret;
    ret = mosqpp::mosquittopp::publish(NULL, topic.c_str(), len, payload);
    if (ret != MOSQ_ERR_SUCCESS)
        spdlog::warn("Could not publish to topic \"{:s}\"", topic.c_str());
    return ret;
}

int Client::publish(const std::string &topic, const std::string &payload)
{
    return publish(topic, payload.c_str(), payload.length());
}

void Client::on_message(const struct mosquitto_message *msg)
{
    std::string topic = msg->topic;

    if (msg->payload == nullptr)
        return ;

    spdlog::trace("Received message: {:s} : {:s}", msg->topic, (char *)msg->payload);

    if (_listeners.count(topic) > 0) {
        spdlog::trace("Found listener for \"{:s}\"", topic);
        _listeners[topic](topic, (char *)msg->payload);
    }
    else {
        forEachListener([&](const std::string &name) {
            if (fnmatch(name.c_str(), topic.c_str(), FNM_PATHNAME) == 0) {
                _listeners[name](topic, (char *)msg->payload);
            }
        });
    }
}

bool Client::registerInterface(Interface &interface, const std::string &name)
{
    bool ret = false;
    std::string &baseTopic = (std::string &)name;

    if (Utils::String::StartsWith(name, "pza/") == false) {
        // is an alias
        if (_alias && _alias->interfaces.count(name) > 0) {
            baseTopic = _alias->interfaces[name];
            interface.setBaseTopic(baseTopic);
        }
        else {
            spdlog::info("Alias {:s} not found.", name);
            return false;
        }
    }
    if (_scanResult.count(baseTopic)) {
        _interfaces[baseTopic] = &interface;
        ret = true;
    }
    return ret;
}

void Client::forEachInterface(const std::function<void(Interface &interface)> &f)
{
    for (auto &item : _interfaces) {
        f(*item.second);
    }
}

void Client::forEachListener(const std::function<void(const std::string&)> &f)
{
    for (auto const &item : _listeners) {
        f(item.first);
    }
}

void Client::on_scan(const std::string &topic, const std::string &payload)
{
    json data;

    data = json::parse(payload);
    if (Utils::Json::KeyExists(data, "info") && Utils::Json::KeyExists(data["info"], "type")) {
        data = data["info"];
        if (data["type"] == "platform" && !Utils::Json::ToInteger(data, "interfaces", _scanCountPlatform)) {
                _scanResult.emplace(topic.substr(0, topic.find("/atts/info")));
                _scanCountInterfaces++;
        }
        else {
            _scanResult.emplace(topic.substr(0, topic.find("/atts/info")));
            _scanCountInterfaces++;
        }
        _cv.notify_one();
    }
}

void Client::showScanResults(void)
{
    spdlog::info("--- Scan results ---");
    spdlog::info("Interface count: {:d}", _scanCountInterfaces);
    spdlog::info("List:");
    for (auto it = _scanResult.begin(); it != _scanResult.end(); it++) {
        spdlog::info("  Interface {:02d}: {:s}", std::distance(_scanResult.begin(), it), *it);
    }
    spdlog::info("--------------------");
}

void Client::scan(int timeout)
{
    std::mutex cv_m;
    std::unique_lock l(cv_m);

    using namespace std::chrono_literals;

    _scanCountInterfaces = 0;
    _scanCountPlatform = 0;
    _scanResult.clear();
    spdlog::info("Start scanning...");
    subscribe("pza/+/+/+/+/info", std::bind(&Client::on_scan, this, std::placeholders::_1, std::placeholders::_2));
    publish("pza", "*");
    auto const _scanComplete = [&](){ return (_scanCountInterfaces != 0 && _scanCountInterfaces == _scanCountPlatform); };
    if (_cv.wait_for(l, std::chrono::seconds(timeout), _scanComplete) == false) {
        if (_scanCountPlatform == 0)
            spdlog::error("No Panduza platform seems to be running on the server.");
        else
            spdlog::warn("Scan didn't finish after {:d} seconds. Found {:d} interfaces but expected {:d}.", timeout, _scanCountInterfaces, _scanCountPlatform);
    }
    else {
        spdlog::info("Scan successful! Found {:d} interfaces.", _scanCountInterfaces);
    }
    showScanResults();
}

Client::~Client()
{
    if (_connected)
        disconnect();
    mosquittopp::loop_stop();
    mosqpp::lib_cleanup();
}