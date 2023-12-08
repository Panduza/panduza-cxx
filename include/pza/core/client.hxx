#pragma once

#include <memory>
#include <optional>
#include <set>
#include <string>

#include <pza/core/device.hxx>

struct client_impl;

namespace pza
{
class client
{
public:
    using s_ptr = std::shared_ptr<client>;
    using u_ptr = std::unique_ptr<client>;
    using w_ptr = std::weak_ptr<client>;

    explicit client(const std::string &addr, int port, std::optional<std::string> id = std::nullopt);
    ~client();

    int connect(void);
    int disconnect(void);
    bool is_connected(void) const;

    const std::string &get_addr(void) const;
    const std::string &get_id(void) const;
    int get_port(void) const;

    void set_connection_timeout(unsigned int timeout_ms);
    unsigned int get_connection_timeout(void) const;

    device::s_ptr register_device(const std::string &group, const std::string &name, unsigned int timeout_ms = device_timeout_default);
    int register_devices(unsigned int timeout_ms = device_timeout_default * 5);

    device::s_ptr get_device(const std::string &group, const std::string &name) const;
    std::vector<device::s_ptr> get_devices() const;
    std::vector<device::s_ptr> get_devices_in_group(const std::string &group) const;
    std::set<std::string> get_groups() const;

    itf_base::s_ptr get_interface(const std::string &group, const std::string &name, const std::string &interface_group, unsigned int idx, const std::string &interface_name) const;
    itf_base::s_ptr get_interface(const std::string &group, const std::string &name, const std::string &interface_name) const;

    template<typename T>
    std::shared_ptr<T> get_interface(const std::string &group, const std::string &name, const std::string &interface_group, unsigned int idx, const std::string &interface_name) const
    {
        return std::dynamic_pointer_cast<T>(get_interface(group, name, interface_group, idx, interface_name));
    }

    template<typename T>
    std::shared_ptr<T> get_interface(const std::string &group, const std::string &name, const std::string &interface_name) const
    {
        return std::dynamic_pointer_cast<T>(get_interface(group, name, interface_name));
    }

private:
    static constexpr unsigned int platforms_timeout_default = 500;
    static constexpr unsigned int device_timeout_default = 500;

    std::unique_ptr<client_impl> _impl;
};
};
