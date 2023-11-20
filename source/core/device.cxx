#include <string>
#include <mutex>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include <pza/core/client.hxx>
#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

using namespace pza;

class device_priv
{
public:
    explicit device_priv(client *client, const struct device_info &info);

    const std::string &get_name() const { return _info.name; }
    const std::string &get_group() const { return _info.group; }
    const std::string &get_model() const { return _info.model; }
    const std::string &get_manufacturer() const { return _info.manufacturer; }
    const std::string &get_family() const { return _info.family; }

    client *get_client() { return _cli; }

    void add_interface(itf::s_ptr itf);

private:
    client *_cli;
    struct device_info _info;
    std::mutex _mtx;
    std::unordered_map<std::string, itf::s_ptr> _interfaces;
};

device_priv::device_priv(client *cli, const struct device_info &info)
    : _cli(cli),
    _info(info)
{

}

void device_priv::add_interface(itf::s_ptr itf)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _interfaces[itf->get_name()] = itf;
}

device::device(client *cli, const device_info &info, configurator cfg)
    : _priv(std::make_unique<device_priv>(cli, info))
{
    cfg(this);
}

device::~device()
{

}

const std::string &device::get_name() const
{
    return _priv->get_name();
}

const std::string &device::get_group() const
{
    return _priv->get_group();
}   

const std::string &device::get_model() const
{
    return _priv->get_model();
}

const std::string &device::get_manufacturer() const
{
    return _priv->get_manufacturer();
}

const std::string &device::get_family() const
{
    return _priv->get_family();
}

client *device::get_client() const
{
    return _priv->get_client();
}

void device::add_interface(itf::s_ptr itf)
{
    _priv->add_interface(itf);
}
