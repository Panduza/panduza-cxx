#include "bps_chan_ctrl.hxx"

#include <iostream>

using namespace pza;

bps_chan_ctrl::bps_chan_ctrl(device *device, const std::string &name)
    : interface(device, name),
    _volts("volts"),
    _amps("amps"),
    _enable("enable")
{
    _volts.add_rw_field<double>("goal");
    _volts.add_ro_field<double>("min");
    _volts.add_ro_field<double>("max");
    _volts.add_ro_field<int>("decimals");

    _amps.add_rw_field<double>("goal");
    _amps.add_ro_field<double>("min");
    _amps.add_ro_field<double>("max");
    _amps.add_ro_field<int>("decimals");

    _enable.add_rw_field<bool>("value");
    _enable.add_rw_field<int>("polling_cycle");
    
    register_attributes({&_volts, &_amps, &_enable});
}

int bps_chan_ctrl::set_voltage(double volts)
{
    double min = _volts.get_field<double>("min").get();
    double max = _volts.get_field<double>("max").get();

    if (volts < min || volts > max) {
        spdlog::error("You can't set voltage to {}, range is {} to {}", volts, min, max);
        return -1;
    }

    return _volts.get_field<double>("goal").set(volts);
}

int bps_chan_ctrl::set_current(double amps)
{
    double min = _amps.get_field<double>("min").get();
    double max = _amps.get_field<double>("max").get();

    if (amps < min || amps > max) {
        spdlog::error("You can't set current to {}, range is {} to {}", amps, min, max);
        return -1;
    }

    return _amps.get_field<double>("goal").set(amps);
}

int bps_chan_ctrl::set_enable(bool enable)
{
    return _enable.get_field<bool>("value").set(enable);
}

bool bps_chan_ctrl::get_enable()
{
    return _enable.get_field<bool>("value").get();
}

int bps_chan_ctrl::set_enable_polling_cycle(double seconds)
{
    return _enable.get_field<int>("polling_cycle").set(seconds);
}

void bps_chan_ctrl::add_enable_callback(const std::function<void(bool)> &callback)
{
    _enable.get_field<bool>("value").add_get_callback(callback);
}

void bps_chan_ctrl::remove_enable_callback(const std::function<void(bool)> &callback)
{
    _enable.get_field<bool>("value").remove_get_callback(callback);
}