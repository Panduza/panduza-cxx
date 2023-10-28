#pragma once

#include <functional>
#include <string>

#include <mqtt/message.h>

class mqtt_service
{
public:
	using s_ptr = std::shared_ptr<mqtt_service>;

	mqtt_service() = default;
	mqtt_service(const mqtt_service &) = delete;
	mqtt_service &operator=(const mqtt_service &) = delete;
	mqtt_service(mqtt_service &&) = delete;
	mqtt_service &operator=(mqtt_service &&) = delete;
	virtual ~mqtt_service() = default;

	virtual int publish(const std::string &topic, const std::string &payload) = 0;
	virtual int publish(mqtt::const_message_ptr msg) = 0;
	virtual int subscribe(const std::string &topic, const std::function<void(mqtt::const_message_ptr)> &cb) = 0;
	virtual int unsubscribe(const std::string &topic) = 0;
};
