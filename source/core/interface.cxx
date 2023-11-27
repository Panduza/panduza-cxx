#include <mutex>
#include <string>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include "attribute.hxx"

using namespace pza;

class itf_impl
{
public:
    itf_impl(device &dev, const std::string &name);

    virtual ~itf_impl() = default;
    itf_impl(const itf_impl&) = delete;
    itf_impl& operator=(const itf_impl&) = delete;
    itf_impl(itf_impl&&) = delete;
    itf_impl& operator=(itf_impl&&) = delete;

    const std::vector<std::string> &get_attributes() const;
    const std::string &get_name() const { return _name; }
    device &get_device() const { return _dev; }

    void on_new_message_ready(const nlohmann::json &data);
    void add_attributes(const std::vector<attribute *> &attributes);

private:    
    device &_dev;
    std::string _name;
    std::string _topic_base;
    std::string _topic_cmd;
};

itf_impl::itf_impl(device &dev, const std::string &name)
    : _name(name),
    _dev(dev),
    _topic_base("pza/" + dev.get_group() + "/" + dev.get_name() + "/" + name),
    _topic_cmd(_topic_base + "/cmds/set")
{

}

itf::itf(device &dev, const std::string &name)
    : _impl(std::make_unique<itf_impl>(dev, name))
{

}

itf::~itf() = default;

const std::string &itf::get_name() const
{
    return _impl->get_name();
}
