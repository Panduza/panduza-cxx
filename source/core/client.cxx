#include <random>
#include <set>

#include <pza/core/client.hxx>
#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include <mqtt/client.h>
#include <spdlog/spdlog.h>

#include "../utils/json_attribute.hxx"
#include "../utils/topic.hxx"
#include "attribute.hxx"
#include "interface_factory.hxx"
#include "mqtt_service.hxx"
#include "scanner.hxx"

using namespace pza;

static constexpr unsigned int conn_timeout_default_ms = 2000;
static constexpr unsigned int msg_timeout_default_ms = 500;
static constexpr unsigned int platforms_timeout_ms = 500;

struct device_info {
	std::string name;
	std::string bench;
	std::string model;
	std::string manufacturer;
	std::string family;
	unsigned int number_of_interfaces = 0;
};

struct client_impl : mqtt_service {
	explicit client_impl(const std::string &addr, int port, std::optional<std::string> id = std::nullopt);

	int connect();
	int disconnect();
	bool is_connected() const
	{
		return _paho_client->is_connected();
	}

	const std::string &get_addr() const
	{
		return _addr;
	}
	const std::string &get_id() const
	{
		return _id;
	}
	int get_port() const
	{
		return _port;
	}

	void set_connection_timeout(unsigned int timeout)
	{
		_conn_timeout = timeout;
	}
	unsigned int get_connection_timeout() const
	{
		return _conn_timeout;
	}

	int publish(const std::string &topic, const std::string &payload) override;
	int publish(mqtt::const_message_ptr msg) override;
	int subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb) override;
	int unsubscribe(const std::string &topic) override;

	void connection_lost(const std::string &cause);
	void message_arrived(mqtt::const_message_ptr msg);

	int scan_platforms(unsigned int timeout_ms);
	int scan_device(const std::string &bench, const std::string &name, unsigned int timeout_ms);
	int scan_devices(unsigned int timeout_ms);

	device::s_ptr create_device(const std::string &bench, const std::string &name);
	device::s_ptr register_device(const std::string &bench, const std::string &name, unsigned int timeout_ms);
	int register_devices(unsigned int timeout_ms);

	device::s_ptr get_device(const std::string &bench, const std::string &name) const;
	std::vector<device::s_ptr> get_devices() const;
	std::vector<device::s_ptr> get_devices_in_bench(const std::string &bench) const;

	std::set<std::string> get_benches() const;

	// std::function for on_device_info
	std::function<void(mqtt::const_message_ptr)> on_device_info;

	mqtt::async_client::ptr_t _paho_client;
	unsigned int _conn_timeout = conn_timeout_default_ms;
	std::string _addr;
	int _port;
	std::string _id;
	std::mutex _mtx;
	std::unordered_map<std::string, std::function<void(mqtt::const_message_ptr)>> _listeners;
	std::unordered_map<std::string, device::s_ptr> _devices;
	std::set<std::string> _platforms_scanned;
	std::unordered_map<std::string, std::string> _devices_scanned;
	unsigned int _device_count = 0;
};

client_impl::client_impl(const std::string &addr, int port, std::optional<std::string> id)
    : _addr(addr),
      _port(port)

{
	std::string url = "tcp://" + addr + ":" + std::to_string(port);

	if (id.has_value() == false) {
		constexpr int range = 1000000;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, range);
		_id = "pza_" + std::to_string(dis(gen));
		spdlog::warn("no client id provided, using generated id: {}", _id);
	} else
		_id = id.value();

	auto message_arrived = [&](mqtt::const_message_ptr msg) {
		spdlog::trace("message arrived on topic: {}", msg->get_topic());

		if (_listeners.count(msg->get_topic()) > 0) {
			_listeners[msg->get_topic()](msg);
			return;
		}

		for (auto &it : _listeners) {
			if (topic::topic_matches(msg->get_topic(), it.first)) {
				it.second(msg);
			}
		}
	};

	auto connection_lost = [&](const std::string &cause) { spdlog::error("connection lost: {}", cause); };

	on_device_info = [&](mqtt::const_message_ptr msg) {
		std::string base_topic = msg->get_topic().substr(4, msg->get_topic().find("/device/atts/info") - 4);

		spdlog::trace("received device info: {} {}", msg->get_topic(), msg->get_payload_str());
		_devices_scanned.emplace(base_topic, msg->get_payload_str());
	};

	_paho_client = std::make_unique<mqtt::async_client>(url, _id);
	_paho_client->set_message_callback(message_arrived);
	_paho_client->set_connection_lost_handler(connection_lost);
	spdlog::trace("created client with id: {}", _id);
}

int client_impl::connect()
{
	mqtt::connect_options connOpts;
	constexpr int interval = 20;

	spdlog::debug("Attempting connection to {}...", _addr);

	connOpts.set_keep_alive_interval(interval);
	connOpts.set_clean_session(true);

	try {
		_paho_client->connect(connOpts)->wait_for(std::chrono::milliseconds(_conn_timeout));
	} catch (const mqtt::exception &exc) {
		spdlog::error("Client {} failed to connect on {}:{}", _id, _addr, _port);
		return -1;
	}

	spdlog::info("connected to {}:{}", _addr, _port);
	return 0;
}

int client_impl::disconnect()
{
	spdlog::debug("Attempting to disconnect from {}...", _addr);

	try {
		_paho_client->disconnect()->wait_for(std::chrono::milliseconds(_conn_timeout));
	} catch (const mqtt::exception &exc) {
		spdlog::error("Client {} failed to disconnect on {}:{}", _id, _addr, _port);
		return -1;
	}
	spdlog::info("disconnected from {}", _addr);
	return 0;
}

void client_impl::connection_lost(const std::string &cause)
{
	spdlog::error("connection lost: {}", cause);
}

int client_impl::publish(const std::string &topic, const std::string &payload)
{
	return publish(mqtt::make_message(topic, payload));
}

int client_impl::publish(mqtt::const_message_ptr msg)
{
	try {
		_paho_client->publish(msg)->wait_for(std::chrono::milliseconds(msg_timeout_default_ms));
	} catch (const mqtt::exception &exc) {
		spdlog::error("Client {} failed to publish", _id);
		return -1;
	}
	spdlog::trace("published message {} to {}", msg->get_payload_str(), msg->get_topic());
	return 0;
}

int client_impl::subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb)
{
	std::string t;

	t = topic::regexify_topic(topic);
	_listeners[t] = cb;
	try {
		_paho_client->subscribe(topic, 0)->wait_for(std::chrono::seconds(_conn_timeout));
	} catch (const mqtt::exception &exc) {
		spdlog::error("Client {} failed to subscribe to topic: {}", _id, topic);
		_listeners.erase(t);
		return -1;
	}

	spdlog::trace("subscribed to topic: {}", topic);
	return 0;
}

int client_impl::unsubscribe(const std::string &topic)
{
	std::string t;

	try {
		_paho_client->unsubscribe(topic)->wait_for(std::chrono::seconds(_conn_timeout));
	} catch (const mqtt::exception &exc) {
		spdlog::error("Client {} failed to unsubscribe from topic: {}", _id, topic);
		return -1;
	}
	spdlog::trace("unsubscribed from topic: {}", topic);
	t = topic::regexify_topic(topic);
	for (auto it = _listeners.begin(); it != _listeners.end();) {
		if (topic::topic_matches(it->first, t)) {
			it = _listeners.erase(it);
		} else
			++it;
	}
	return 0;
}

void client_impl::message_arrived(mqtt::const_message_ptr msg)
{
	spdlog::trace("message arrived on topic: {}", msg->get_topic());

	if (_listeners.count(msg->get_topic()) > 0) {
		_listeners[msg->get_topic()](msg);
		return;
	}

	for (auto &it : _listeners) {
		if (topic::topic_matches(msg->get_topic(), it.first)) {
			it.second(msg);
		}
	}
}

int client_impl::scan_platforms(unsigned int timeout_ms)
{
	scanner scanner(this);

	_platforms_scanned.clear();

	auto on_platform_info = [&](mqtt::const_message_ptr msg) {
		const std::string &payload = msg->get_payload_str();
		const std::string &topic = msg->get_topic();
		std::string type;
		unsigned int val = 0;
		json_attribute json("info");

		if (json.parse(payload) < 0) {
			spdlog::error("failed to parse platform info: {}", payload);
			return;
		}

		if (json.get_string("type", type) < 0) {
			spdlog::error("failed to parse type info: {}", payload);
			return;
		}
		if (type != "platform" || _platforms_scanned.find(topic) != _platforms_scanned.end())
			return;

		_platforms_scanned.insert(topic);

		spdlog::trace("received platform info: {}", payload);

		if (json.get_unsigned_int("number_of_devices", val) < 0) {
			spdlog::error("failed to parse platform info: {}", payload);
			return;
		}
		_device_count += val;
	};

	scanner.set_scan_timeout_ms(timeout_ms)
	    .set_message_callback(on_platform_info)
	    .set_condition_callback([&]() {
		    std::this_thread::sleep_for(std::chrono::milliseconds(platforms_timeout_ms));
		    return true;
	    })
	    .set_publisher(mqtt::make_message("pza", "p"))
	    .set_subscription_topic("pza/server/+/+/atts/info")
	    .run();

	if (_platforms_scanned.empty()) {
		spdlog::error("No platforms found");
		return -1;
	}

	if (_device_count == 0) {
		spdlog::error("No devices found on scanned platforms");
		return -1;
	}
	spdlog::info("Found {} platform{} with {} device{}", _platforms_scanned.size(),
		     _platforms_scanned.size() > 1 ? "s" : "", _device_count, _device_count > 1 ? "s" : "");
	return 0;
}

int client_impl::scan_devices(unsigned int timeout_ms)
{
	scanner scanner(this);

	_devices_scanned.clear();

	auto on_device_info = [&](mqtt::const_message_ptr msg) {
		std::string base_topic = msg->get_topic().substr(4, msg->get_topic().find("/device/atts/info") - 4);

		spdlog::trace("received device info: {} {}", msg->get_topic(), msg->get_payload_str());
		_devices_scanned.emplace(base_topic, msg->get_payload_str());
	};

	scanner.set_scan_timeout_ms(timeout_ms)
	    .set_message_callback(on_device_info)
	    .set_condition_callback([&]() { return (_device_count == _devices_scanned.size()); })
	    .set_publisher(mqtt::make_message("pza", "d"))
	    .set_subscription_topic("pza/+/+/device/atts/info");

	if (scanner.run() < 0) {
		spdlog::error("timed out waiting for devices");
		return -1;
	}
	return 0;
}

int client_impl::scan_device(const std::string &bench, const std::string &name, unsigned int timeout_ms)
{
	scanner scanner(this);
	auto combined = bench + "/" + name;

	_devices.erase(combined);

	scanner.set_scan_timeout_ms(timeout_ms)
	    .set_message_callback(on_device_info)
	    .set_condition_callback([&]() { return _devices_scanned.count(combined) > 0; })
	    .set_publisher(mqtt::make_message("pza", combined))
	    .set_subscription_topic("pza/" + combined + "/device/atts/info");

	if (scanner.run() < 0) {
		spdlog::error("timed out waiting for device {}", combined);
		return -1;
	}
	return 0;
}

device::s_ptr client_impl::create_device(const std::string &bench, const std::string &name)
{
	device::s_ptr dev;
	json_attribute json("info");
	device_info info{};
	auto combined = bench + "/" + name;

	auto elem = _devices_scanned.at(combined);

	if (json.parse(elem) < 0) {
		spdlog::error("failed to parse device info for device {}", combined);
		return nullptr;
	}

	if (json.get_unsigned_int("number_of_interfaces", info.number_of_interfaces) < 0) {
		spdlog::error("failed to parse number of interfaces for device {}", combined);
		return nullptr;
	}

	info.bench = bench;
	info.name = name;

	try {
		dev = std::make_shared<device>(this, info);
	} catch (const std::exception &exc) {
		spdlog::error("failed to create device: {}", exc.what());
		return nullptr;
	}
	_devices[combined] = dev;
	return dev;
}

device::s_ptr client_impl::register_device(const std::string &bench, const std::string &name, unsigned int timeout_ms)
{
	if (scan_device(bench, name, timeout_ms) < 0) {
		spdlog::error("failed to scan device {}", name);
		return nullptr;
	}
	return create_device(bench, name);
}

int client_impl::register_devices(unsigned int timeout_ms)
{
	int ret = 0;
	device_info info;

	if (scan_platforms(timeout_ms) < 0) {
		spdlog::error("failed to scan platforms.");
		return -1;
	}
	if (scan_devices(timeout_ms) < 0) {
		spdlog::error("failed to scan devices.");
		return -1;
	}
	for (auto const &elem : _devices_scanned) {
		auto bench = elem.first.substr(0, elem.first.find('/'));
		auto name = elem.first.substr(elem.first.find('/') + 1);
		if (create_device(bench, name) == nullptr) {
			spdlog::error("failed to create device {}", elem.first);
			ret = -1;
		}
	}
	return ret;
}

device::s_ptr client_impl::get_device(const std::string &bench, const std::string &name) const
{
	auto combined = bench + "/" + name;

	if (_devices.count(combined) == 0)
		return nullptr;
	return _devices.at(combined);
}

std::vector<device::s_ptr> client_impl::get_devices() const
{
	std::vector<device::s_ptr> ret;

	for (auto const &elem : _devices) {
		ret.push_back(elem.second);
	}
	return ret;
}

std::vector<device::s_ptr> client_impl::get_devices_in_bench(const std::string &bench) const
{
	std::vector<device::s_ptr> ret;

	for (auto const &elem : _devices) {
		if (elem.second->get_bench() == bench)
			ret.push_back(elem.second);
	}
	return ret;
}

std::set<std::string> client_impl::get_benches() const
{
	std::set<std::string> benches;

	for (auto const &elem : _devices) {
		benches.insert(elem.second->get_bench());
	}
	return benches;
}

client::client(const std::string &addr, int port, std::optional<std::string> id)
    : _impl(std::make_unique<client_impl>(addr, port, id))
{
}

client::~client() = default;

int client::connect()
{
	return _impl->connect();
}

int client::disconnect()
{
	return _impl->disconnect();
}

bool client::is_connected() const
{
	return _impl->is_connected();
}

const std::string &client::get_addr() const
{
	return _impl->get_addr();
}

const std::string &client::get_id() const
{
	return _impl->get_id();
}

int client::get_port() const
{
	return _impl->get_port();
}

void client::set_connection_timeout(unsigned int timeout)
{
	_impl->set_connection_timeout(timeout);
}

unsigned int client::get_connection_timeout() const
{
	return _impl->get_connection_timeout();
}

device::s_ptr client::register_device(const std::string &bench, const std::string &name, unsigned int timeout_ms)
{
	return _impl->register_device(bench, name, timeout_ms);
}

int client::register_devices(unsigned int timeout_ms)
{
	return _impl->register_devices(timeout_ms);
}

device::s_ptr client::get_device(const std::string &bench, const std::string &name) const
{
	return _impl->get_device(bench, name);
}

std::vector<device::s_ptr> client::get_devices() const
{
	return _impl->get_devices();
}

std::vector<device::s_ptr> client::get_devices_in_bench(const std::string &bench) const
{
	return _impl->get_devices_in_bench(bench);
}

std::set<std::string> client::get_benches() const
{
	return _impl->get_benches();
}

itf_base::s_ptr client::get_interface(const std::string &bench, const std::string &device,
				      const std::string &interface_group, unsigned int idx,
				      const std::string &name) const
{
	auto dev = _impl->get_device(bench, device);
	if (dev == nullptr)
		return nullptr;
	return dev->get_interface(interface_group, idx, name);
}

itf_base::s_ptr client::get_interface(const std::string &bench, const std::string &device,
				      const std::string &name) const
{
	auto dev = _impl->get_device(bench, device);
	if (dev == nullptr)
		return nullptr;
	return dev->get_interface(name);
}

std::vector<itf_base::s_ptr> client::get_interfaces() const
{
	auto dev = _impl->get_devices();
	std::vector<itf_base::s_ptr> ret;

	for (auto const &elem : dev) {
		auto itf = elem->get_interfaces();
		ret.insert(ret.end(), itf.begin(), itf.end());
	}

	return ret;
}
