#pragma once

#include <magic_enum.hpp>

#include <pza/core/device.hxx>
#include <pza/devices/bps.hxx>

namespace pza
{
    class device_factory
    {
    public:
        device_factory() = delete;
        ~device_factory() = delete;
        device_factory(const device_factory &) = delete;
        device_factory &operator=(const device_factory &) = delete;

        static device::ptr create_device(const std::string &family, const std::string &group, const std::string &name);

        template <typename T>
        static device::ptr allocate_device(const std::string &group, const std::string &name)
        {
            return std::make_shared<T>(group, name);
        }

    private:
        using factory_function = std::function<device::ptr(const std::string &group, const std::string &name)>;
        static std::map<std::string, factory_function> _factory_map;
    };
};