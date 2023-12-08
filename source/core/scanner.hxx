#pragma once

#include <condition_variable>
#include <mutex>

#include <spdlog/spdlog.h>

#include "mqtt_service.hxx"

class scanner
{
public:
	explicit scanner(mqtt_service &mqtt);

	scanner &set_scan_timeout_ms(unsigned int timeout)
	{
		_scan_timeout = timeout;
		return *this;
	}
	scanner &set_message_callback(
	    const std::function<void(mqtt::const_message_ptr)> &cb)
	{
		_message_cb = cb;
		return *this;
	}
	scanner &set_condition_callback(const std::function<bool()> &cb)
	{
		_condition_cb = cb;
		return *this;
	}
	scanner &set_subscription_topic(const std::string &topic)
	{
		_sub_topic = topic;
		return *this;
	}
	scanner &set_publisher(mqtt::const_message_ptr msg)
	{
		_pub_msg = msg;
		return *this;
	}
	int run();

private:
	static constexpr unsigned int _scan_timeout_default = 3; // in seconds
	unsigned int _scan_timeout = _scan_timeout_default;
	mqtt_service &_mqtt;
	std::function<void(mqtt::const_message_ptr)> _message_cb;
	std::function<bool()> _condition_cb;
	std::condition_variable _cv;
	std::mutex _mtx;
	std::string _sub_topic;
	mqtt::const_message_ptr _pub_msg;
};
