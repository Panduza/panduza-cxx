#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include <nlohmann/json.hpp>

class itf_impl;
class attribute;

namespace pza
{
class device;

// We can't use the name interface because it's a reserved keyford for Windows C++ (lol)
class itf
{
public:
    using s_ptr = std::shared_ptr<itf>;
    using u_ptr = std::unique_ptr<itf>;
    using w_ptr = std::weak_ptr<itf>;

    virtual ~itf();
    itf(const itf&) = delete;
    itf& operator=(const itf&) = delete;
    itf(itf&&) = delete;
    itf& operator=(itf&&) = delete;

    const std::string &get_name() const;

protected:
    explicit itf(device &dev, const std::string &name);

    void add_attributes(const std::vector<attribute *> &attributes);

    std::unique_ptr<itf_impl> _impl;
};
};