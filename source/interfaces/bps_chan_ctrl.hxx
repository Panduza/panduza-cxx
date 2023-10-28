#pragma once

#include <functional>
#include <memory>
#include <variant>

#include "../core/attribute.hxx"
#include "../core/interface.hxx"

class bps_chan_ctrl : public itf
{
public:
    using ptr = std::shared_ptr<bps_chan_ctrl>;

    bps_chan_ctrl(device *device, const std::string &name);

    int set_voltage(double volts);
    int set_current(double amps);
    int set_enable(bool enable);
    int set_enable_polling_cycle(double seconds);
    bool get_enable();

    void add_enable_callback(const std::function<void(bool)> &callback);
    void remove_enable_callback(const std::function<void(bool)> &callback);

private:
    attribute _att_voltage;
    attribute _att_current;
    attribute _enable;
};