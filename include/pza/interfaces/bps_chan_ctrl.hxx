#pragma once

#include <functional>
#include <memory>
#include <string>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

class attribute;

namespace pza
{
class bps_chan_ctrl : public itf
{
public:
    using s_ptr = std::shared_ptr<bps_chan_ctrl>;
    using u_ptr = std::unique_ptr<bps_chan_ctrl>;
    using w_ptr = std::weak_ptr<bps_chan_ctrl>;

    bps_chan_ctrl(device *dev, const std::string &name, client_callbacks cb);
    ~bps_chan_ctrl();

    int set_voltage(double volts);
    int set_current(double amps);
    int set_enable(bool enable);
    int set_enable_polling_cycle(double seconds);
    bool get_enable();

    void add_enable_callback(const std::function<void(bool)> &callback);
    void remove_enable_callback(const std::function<void(bool)> &callback);

private:
    attribute *_att_voltage;
    attribute *_att_current;
    attribute *_enable;
};
};