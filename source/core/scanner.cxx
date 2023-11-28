#include "scanner.hxx"

scanner::scanner(mqtt_service &mqtt)
    : _mqtt(mqtt),
    _message_cb(nullptr),
    _condition_cb(nullptr),
    _pub_msg(nullptr)
{

}

int scanner::run()
{
    bool ret;

    std::unique_lock<std::mutex> lock(_mtx);

    if (_message_cb == nullptr || _condition_cb == nullptr || _pub_msg == nullptr || _sub_topic.empty()) {
        spdlog::error("scanner not configured");
        return -1;
    }

    _mqtt.subscribe(_sub_topic, std::bind(&scanner::_on_message, this, std::placeholders::_1));
    _mqtt.publish(_pub_msg);

    ret = _cv.wait_for(lock, std::chrono::seconds(_scan_timeout), _condition_cb);

    _mqtt.unsubscribe(_sub_topic);
    
    return (ret == true) ? 0 : -1;
}

void scanner::_on_message(mqtt::const_message_ptr msg)
{
    _message_cb(msg);
    _cv.notify_one();
}