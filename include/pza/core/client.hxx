#pragma once

#include <memory>
#include <optional>
#include <string>

#include <pza/core/device.hxx>

class client_impl;

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

    void set_conn_timeout(int timeout);
    int get_conn_timeout(void) const;

    int scan();

    device::s_ptr register_device(const std::string &group, const std::string &name);

private:
    std::unique_ptr<client_impl> _impl;
};
};