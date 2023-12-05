#pragma once

#include <pza/core/interface.hxx>

namespace pza::itf
{
class platform : public itf_base
{
public:
    using s_ptr = std::shared_ptr<platform>;
    using u_ptr = std::unique_ptr<platform>;
    using w_ptr = std::weak_ptr<platform>;
    
    explicit platform(mqtt_service &mqtt, itf_info &info);
    ~platform();

private:
};
};