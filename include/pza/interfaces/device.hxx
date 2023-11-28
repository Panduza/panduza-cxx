#pragma once

#include <memory>
#include <functional>
#include <string>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

namespace pza::itf
{
class device : public itf_base
{
public:
    using s_ptr = std::shared_ptr<device>;
    using u_ptr = std::unique_ptr<device>;
    using w_ptr = std::weak_ptr<device>;
    
    device(mqtt_service &mqtt, itf_info &info);
    ~device();

private:
    std::unique_ptr<attribute> _identity;
};
};