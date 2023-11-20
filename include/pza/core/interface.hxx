#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include <nlohmann/json.hpp>

class itf_priv;
class attribute;

namespace pza
{
class device;

// We can't use the name interface because it's a reserved keyford for Windows C++ (lol)
class itf
{
public:
    using s_ptr = std::shared_ptr<itf>;

    struct client_callbacks
    {
        std::function<void(const std::string &, const std::vector<attribute *> &)> on_new_attributes;
        std::function<void(const std::string &, const std::string &)> on_new_message;
    };

    virtual ~itf();
    itf(const itf&) = delete;
    itf& operator=(const itf&) = delete;
    itf(itf&&) = delete;
    itf& operator=(itf&&) = delete;

    const std::string &get_name() const;
    device *get_device() const;

protected:
    explicit itf(device *dev, const std::string &name, client_callbacks cb);

    void add_attributes(const std::vector<attribute *> &attributes);

    std::unique_ptr<itf_priv> _priv;
};
};