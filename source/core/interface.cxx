#include <mutex>
#include <string>

#include <mqtt/message.h>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include "attribute.hxx"
#include "interface.hxx"
#include "mqtt_service.hxx"

using namespace pza;

static constexpr unsigned int attributes_timeout = 2;

itf_impl::itf_impl(mqtt_service &mqtt, itf_info &info)
    : info(info),
    topic_base("pza/" + info.group + "/" + info.device_name + "/" + info.name),
    topic_cmd(topic_base + "/cmds/set"),
    mqtt(mqtt)
{

}

itf_impl::~itf_impl()
{
    for (auto const &elem : attributes) {
        auto att = elem.second.get();
        std::string att_topic = topic_base + "/atts/" + att->get_name();

        mqtt.unsubscribe(att_topic);
    }
}

void itf_impl::register_attributes(const std::list<attribute::s_ptr> &attribute_list)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    std::condition_variable cv;
    unsigned int count = 0;

    for (auto const &att : attribute_list) {
        const std::string &att_name = att->get_name();
        std::string att_topic = topic_base + "/atts/" + att_name;

        attributes[att_name] = att;
        mqtt.subscribe(att_topic, [&](mqtt::const_message_ptr msg) {
            att->on_message(msg);
            count++;
            cv.notify_one();
        });
    }

    if (cv.wait_for(lock, std::chrono::seconds(attributes_timeout), [&]() { return (count == attributes.size()); }) == false) {
        throw std::runtime_error("timed out waiting for attributes");
    }

    for (auto const &elem : attributes) {
        auto att = elem.second;
        std::string att_topic = topic_base + "/atts/" + att->get_name();

        att->set_msg_callback(std::bind(&itf_impl::send_message, this, std::placeholders::_1));
        mqtt.subscribe(att_topic, std::bind(&attribute::on_message, att, std::placeholders::_1));

    }
}

attribute &itf_impl::get_attribute(const std::string &name)
{
    return *attributes[name];
}

int itf_impl::send_message(const nlohmann::json &data)
{
    return mqtt.publish(mqtt::make_message(topic_cmd, data.dump()));
}

itf_base::itf_base(mqtt_service &mqtt, itf_info &info)
    : _impl(std::make_unique<itf_impl>(mqtt, info))
{

}

itf_base::~itf_base()
{
}

const std::string &itf_base::get_name() const
{
    return _impl->get_name();
}

const std::string &itf_base::get_type() const
{
    return _impl->get_type();
}
