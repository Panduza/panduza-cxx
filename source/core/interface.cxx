#include <mutex>
#include <string>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include "attribute.hxx"

using namespace pza;

class itf_priv
{
public:
    using attribute_map = std::unordered_map<std::string, attribute*>;

    itf_priv(device *dev, const std::string &name, itf::client_callbacks &cb);

    virtual ~itf_priv() = default;
    itf_priv(const itf_priv&) = delete;
    itf_priv& operator=(const itf_priv&) = delete;
    itf_priv(itf_priv&&) = delete;
    itf_priv& operator=(itf_priv&&) = delete;

    const std::vector<std::string> &get_attributes() const;
    const std::string &get_name() const { return _name; }
    device *get_device() const { return _dev; }

    void on_new_message_ready(const nlohmann::json &data);
    void add_attributes(const std::vector<attribute *> &attributes);

private:
    std::mutex _mtx;
    device *_dev;
    std::string _name;
    std::string _topic_base;
    std::string _topic_cmd;
    attribute_map _attributes;
    itf::client_callbacks _cb;
};

itf_priv::itf_priv(device *dev, const std::string &name, itf::client_callbacks &cb)
    : _dev(dev),
    _name(name),
    _topic_base("pza/" + dev->get_group() + "/" + dev->get_name() + "/" + name),
    _topic_cmd(_topic_base + "/cmds/set"),
    _cb(cb)
{

}

void itf_priv::add_attributes(const std::vector<attribute *> &attributes)
{
    _cb.on_new_attributes(_topic_base, attributes);
    for (auto &it : attributes) {
        it->set_callback(std::bind(&itf_priv::on_new_message_ready, this, std::placeholders::_1));
        _attributes[it->get_name()] = it;
    }
}

inline void itf_priv::on_new_message_ready(const nlohmann::json &data)
{
    _cb.on_new_message(_topic_cmd, data.dump());
}

itf::itf(device *dev, const std::string &name, client_callbacks cb)
    : _priv(std::make_unique<itf_priv>(dev, name, cb))
{

}

itf::~itf() = default;

const std::string &itf::get_name() const
{
    return _priv->get_name();
}

device *itf::get_device() const
{
    return _priv->get_device();
}

void itf::add_attributes(const std::vector<attribute *> &attributes)
{
    _priv->add_attributes(attributes);
}