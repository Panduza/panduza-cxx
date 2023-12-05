#pragma once

#include <functional>

#include <spdlog/spdlog.h>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

#include "interface.hxx"

using namespace pza;

namespace interface_factory
{
    using factory_function = std::function<pza::itf_base::s_ptr(mqtt_service &mqtt, itf_info &info)>;

    itf_base::s_ptr create_interface(mqtt_service &mqtt, const std::string &group, const std::string &device_name, const std::string &name, const std::string &type);
};