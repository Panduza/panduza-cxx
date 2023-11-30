#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include <nlohmann/json.hpp>

class itf_impl;
class attribute;
class mqtt_service;

namespace pza
{
class device;

struct itf_info
{
    std::string group;
    std::string device_name;
    std::string name;
    std::string type;
};

// We can't use the name interface because it's a reserved keyford for Windows C++ (lol)
class itf_base
{
public:
    using s_ptr = std::shared_ptr<itf_base>;
    using u_ptr = std::unique_ptr<itf_base>;
    using w_ptr = std::weak_ptr<itf_base>;

    using attribute_ptr = std::unique_ptr<attribute>;

    virtual ~itf_base();
    itf_base(const itf_base&) = delete;
    itf_base& operator=(const itf_base&) = delete;
    itf_base(itf_base&&) = delete;
    itf_base& operator=(itf_base&&) = delete;

    const std::string &get_name() const;

protected:
    explicit itf_base(mqtt_service &mqtt, itf_info &info);

    void register_attributes(const std::vector<std::reference_wrapper<attribute>> attributes);

    std::unique_ptr<itf_impl> _impl;
};
};