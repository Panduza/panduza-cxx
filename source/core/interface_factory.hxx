#pragma once

#include <functional>

#include <spdlog/spdlog.h>

#include "interface.hxx"

namespace interface_factory
{
    using factory_function = std::function<itf::ptr(device *dev, const std::string &name)>;

    itf::ptr create_interface(device *dev, const std::string &name, const std::string &type);

    template <typename T>
    itf::ptr allocate_interface(device *dev, const std::string &name)
    {
        return std::make_shared<T>(dev, name);
    }
};