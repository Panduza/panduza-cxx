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
    const Core::Alias *alias;
    
    alias = Core::Get().findAlias(aliasName);
    if (alias) {
        _alias = alias;
        _addr = alias->url;
        _port = alias->port;
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
            discover();
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

int Client::subscribe(const std::string &topic, const std::function<void(const std::string &payload)> &f)
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

    if (_listeners.count(topic) > 0) {
        spdlog::trace("Found listener for \"{:s}\"", topic);
        _listeners[topic]((char *)msg->payload);
    }
    else {
        forEachListener([&](const std::string &name) {
            if (fnmatch(name.c_str(), topic.c_str(), FNM_PATHNAME) == 0)
                _listeners[name]((char *)msg->payload);
        });
    }
}

bool Client::interfaceIsRegistered(const std::string &topic)
{
    return (_interfaces.count(topic) != 0) ? true : false;
}

void Client::forEachInterface(const std::function<void(Interface &interface)> &f)
{
    for (auto &item : _interfaces) {
        f(item.second);
    }
}

void Client::forEachListener(const std::function<void(const std::string&)> &f)
{
    for (auto &item : _listeners) {
        f(item.first);
    }
}

void Client::on_info(const std::string &payload)
{
    printf("%p\n", this);
}

void Client::discover(void)
{
    subscribe("pza/+/+/+/atts/info", std::bind(&Client::on_info, this, std::placeholders::_1));
    publish("pza", "*");
}

Client::~Client()
{
    if (_connected)
        disconnect();
    mosquittopp::loop_stop();
    mosqpp::lib_cleanup();
}