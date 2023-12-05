#pragma once

#include <functional>
#include <memory>

#include <pza/core/interface.hxx>

class device_impl;
class mqtt_service;

struct device_info;
namespace pza
{
class client;


class device
{
public:
    using s_ptr = std::shared_ptr<device>;
    using u_ptr = std::unique_ptr<device>;
    using w_ptr = std::weak_ptr<device>;

    explicit device(mqtt_service &mqtt, struct device_info &info);
    ~device();

    const std::string &get_name() const;
    const std::string &get_group() const;
    const std::string &get_model() const;
    const std::string &get_manufacturer() const;
    const std::string &get_family() const;

    int get_number_of_interfaces() const;
    
    itf_base::s_ptr get_interface(const std::string &name) const;
    itf_base::s_ptr get_interface(const std::string &interface_group, unsigned int idx, const std::string &name) const;

    std::vector<std::string> get_interface_names() const;

    unsigned int get_interface_group_count(const std::string &group) const;

private:
    std::unique_ptr<device_impl> _impl;
};
};