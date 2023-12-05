#include <mutex>
#include <string>

#include <mqtt/message.h>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include "attribute.hxx"
#include "mqtt_service.hxx"

using namespace pza;

static constexpr unsigned int attributes_timeout = 2;

struct itf_impl
{
    explicit itf_impl(mqtt_service &mqtt, itf_info &info);

    virtual ~itf_impl();
    itf_impl(const itf_impl&) = delete;
    itf_impl& operator=(const itf_impl&) = delete;
    itf_impl(itf_impl&&) = delete;
    itf_impl& operator=(itf_impl&&) = delete;

    const std::string &get_name() const { return info.name; }
    const std::string &get_type() const { return info.type; }

    void register_attributes(const std::list<std::reference_wrapper<attribute>> attributes);

    int send_message(const nlohmann::json &data);
    
    itf_info info;
    std::string topic_base;
    std::string topic_cmd;
    mqtt_service &mqtt;
    std::vector<std::string> sub_topic_list;
};

itf_impl::itf_impl(mqtt_service &mqtt, itf_info &info)
    : info(info),
    topic_base("pza/" + info.group + "/" + info.device_name + "/" + info.name),
    topic_cmd(topic_base + "/cmds/set"),
    mqtt(mqtt)
{

}

itf_impl::~itf_impl()
{
    spdlog::trace("itf_impl::~itf_impl");
    for (auto &topic : sub_topic_list) {
        spdlog::trace("itf_impl::~itf_impl: unsubscribing from {}", topic);
        mqtt.unsubscribe(topic);
    }
}

void itf_impl::register_attributes(const std::list<std::reference_wrapper<attribute>> attributes)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    std::condition_variable cv;
    unsigned int count = 0;

    for (auto wrapper : attributes) {
       auto &att = wrapper.get();
       std::string att_topic = topic_base + "/atts/" + att.get_name();
       mqtt.subscribe(att_topic, [&](mqtt::const_message_ptr msg) {
           att.on_message(msg);
           count++;
           cv.notify_one();
       });
    }

    if (cv.wait_for(lock, std::chrono::seconds(attributes_timeout), [&]() { return (count == attributes.size()); }) == false) {
        throw std::runtime_error("timed out waiting for attributes");
    }

    sub_topic_list.reserve(count);

    for (auto wrapper : attributes) {
       auto &att = wrapper.get();
       std::string att_topic = topic_base + "/atts/" + att.get_name();
       att.set_msg_callback(std::bind(&itf_impl::send_message, this, std::placeholders::_1));
       mqtt.subscribe(att_topic, std::bind(&attribute::on_message, &att, std::placeholders::_1));
       sub_topic_list.emplace_back(att_topic);

    }
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
    spdlog::trace("itf_base::~itf_base");
}

const std::string &itf_base::get_name() const
{
    return _impl->get_name();
}

const std::string &itf_base::get_type() const
{
    return _impl->get_type();
}

void itf_base::register_attributes(const std::list<std::reference_wrapper<attribute>> attributes)
{
    _impl->register_attributes(attributes);
}