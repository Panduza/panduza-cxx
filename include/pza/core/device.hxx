#pragma once

#include <functional>
#include <memory>

#include <pza/core/interface.hxx>

class device_impl;
class mqtt_service;

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
    unsigned int number_of_interfaces;
};

class device
{
public:
    using s_ptr = std::shared_ptr<device>;
    using u_ptr = std::unique_ptr<device>;
    using w_ptr = std::weak_ptr<device>;

    explicit device(mqtt_service &mqtt, const struct device_info &info);
    ~device();

    const std::string &get_name() const;
    const std::string &get_group() const;
    const std::string &get_model() const;
    const std::string &get_manufacturer() const;
    const std::string &get_family() const;

private:
    std::unique_ptr<device_impl> _impl;
};
};