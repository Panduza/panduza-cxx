#include "interface.hxx"
#include <pza/core/device.hxx>
#include <pza/core/client.hxx>

using namespace pza;

itface::itface(device *device, const std::string &name)
    : _device(device),
    _name(name)
{
    _topic_base = _device->_get_base_topic() + "/" + _name;
    _topic_cmd = _topic_base + "/cmds/set";
}

void itface::register_attribute(attribute &attribute)
{
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(_mtx);
    bool received = false;
    std::string topic = _topic_base + "/atts/" + attribute._name;

    _device->get_client()->_subscribe(topic, [&](const mqtt::const_message_ptr &msg) {
        attribute.on_message(msg);
        attribute._callback = [&](const nlohmann::json &data) {
            _device->get_client()->_publish(_topic_cmd, data.dump());
        };
        _attributes[attribute._name] = &attribute;
        received = true;
        cv.notify_one();
    });

    if (cv.wait_for(lock, std::chrono::seconds(5), [&]() { return received; }) == false) {
        spdlog::error("timed out waiting for attribute registration ({})", topic);
    }
    _device->get_client()->_unsubscribe(topic);
    if (received) {
        _device->get_client()->_subscribe(topic, std::bind(&attribute::on_message, &attribute, std::placeholders::_1));
    }
}

void itface::register_attributes(const std::vector<attribute*> &list)
{
    for (auto const &it : list) {
        register_attribute(*it);
    }
}