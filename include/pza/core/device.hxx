#pragma once

#include <memory>
#include <functional>

#include <pza/core/interface.hxx>

class device_priv;

namespace pza
{
class client;

struct device_info
{
    std::string name;
    std::string group;
    std::string model;
    std::string manufacturer;
    std::string family;
};

class device
{
public:
    using s_ptr = std::shared_ptr<device>;
    using u_ptr = std::unique_ptr<device>;
    using w_ptr = std::weak_ptr<device>;

    using configurator = std::function<void(device *)>;

    explicit device(pza::client *client, const device_info &inf, configurator cfg);
    ~device();

    const std::string &get_name() const;
    const std::string &get_group() const;
    const std::string &get_model() const;
    const std::string &get_manufacturer() const;
    const std::string &get_family() const;
    pza::client *get_client() const;

    void add_interface(itf::s_ptr itf);

private:
    std::unique_ptr<device_priv> _priv;
};
};