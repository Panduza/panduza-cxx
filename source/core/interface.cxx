#include <mutex>
#include <string>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include <mqtt/message.h>

#include "attribute.hxx"
#include "mqtt_service.hxx"

using namespace pza;

struct itf_impl
{
    explicit itf_impl(mqtt_service &mqtt, itf_info &info);

    virtual ~itf_impl() = default;
    itf_impl(const itf_impl&) = delete;
    itf_impl& operator=(const itf_impl&) = delete;
    itf_impl(itf_impl&&) = delete;
    itf_impl& operator=(itf_impl&&) = delete;

    const std::vector<std::string> &get_attributes() const;
    const std::string &get_name() const { return info.name; }

    void register_attributes(const std::vector<std::reference_wrapper<attribute>> attributes);

    itf_info &info;
    std::string topic_base;
    std::string topic_cmd;
    mqtt_service &mqtt;
};

itf_impl::itf_impl(mqtt_service &mqtt, itf_info &info)
    : info(info),
    topic_base("pza/" + info.group + "/" + info.device_name + "/" + info.name),
    topic_cmd(topic_base + "/cmds/set"),
    mqtt(mqtt)
{

}

void itf_impl::register_attributes(const std::vector<std::reference_wrapper<attribute>> attributes)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    std::condition_variable cv;
    unsigned int n = 0;

    for (auto wrapper : attributes) {
       auto &att = wrapper.get();
       std::string att_topic = topic_base + "/atts/" + att.get_name();
       mqtt.subscribe(att_topic, [&](mqtt::const_message_ptr msg) {
           att.on_message(msg);
           n++;
           cv.notify_one();
       });
    }

    cv.wait_for(lock, std::chrono::seconds(5), [&](){
       return (n == attributes.size());
    });

    for (auto wrapper : attributes) {
       auto &att = wrapper.get();
       std::string att_topic = topic_base + "/atts" + att.get_name();
       mqtt.subscribe(att_topic, std::bind(&attribute::on_message, &att, std::placeholders::_1));
    }
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

void itf_base::register_attributes(const std::vector<std::reference_wrapper<attribute>> attributes)
{
    _impl->register_attributes(attributes);
}