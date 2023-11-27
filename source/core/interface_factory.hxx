#pragma once

#include <functional>

#include <spdlog/spdlog.h>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

namespace interface_factory
{
    using factory_function = std::function<pza::itf::s_ptr(pza::device &dev, const std::string &name)>;

    pza::itf::s_ptr create_interface(pza::device &dev, const std::string &name, const std::string &type);
};