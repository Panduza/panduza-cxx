#include <pza/interfaces/bps_chan_ctrl.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

bps_chan_ctrl::bps_chan_ctrl(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _voltage = std::make_unique<attribute>("voltage");
    _current = std::make_unique<attribute>("current");
    _enable = std::make_unique<attribute>("enable");

    _voltage->register_field<double>("value");
    _voltage->register_field<double>("min");
    _voltage->register_field<double>("max");
    _voltage->register_field<unsigned int>("decimals");

    _current->register_field<double>("value");
    _current->register_field<double>("min");
    _current->register_field<double>("max");
    _current->register_field<unsigned int>("decimals");

    _enable->register_field<bool>("value");

    register_attributes({*_voltage, *_current, *_enable});
}

bps_chan_ctrl::~bps_chan_ctrl()
{
    spdlog::trace("bps_chan_ctrl::~bps_chan_ctrl");
}

int bps_chan_ctrl::set_voltage(double volts)
{
    return _voltage->set<double>("value", volts);
}

int bps_chan_ctrl::set_current(double amps)
{
    return _current->set<double>("value", amps);
}

int bps_chan_ctrl::set_enable(bool enable)
{
    return _enable->set<bool>("value", enable);
}

bool bps_chan_ctrl::get_enable()
{
    return _enable->get_field<bool>("value");
}

double bps_chan_ctrl::get_min_voltage()
{
    return _voltage->get_field<double>("min");
}

double bps_chan_ctrl::get_max_voltage()
{
    return _voltage->get_field<double>("max");
}

double bps_chan_ctrl::get_min_current()
{
    return _current->get_field<double>("min");
}

double bps_chan_ctrl::get_max_current()
{
    return _current->get_field<double>("max");
}

unsigned int bps_chan_ctrl::get_num_decimals_voltage()
{
    return _voltage->get_field<unsigned int>("decimals");
}

unsigned int bps_chan_ctrl::get_num_decimals_current()
{
    return _current->get_field<unsigned int>("decimals");
}

void bps_chan_ctrl::register_enable_callback(const std::function<void()> &callback)
{
    _enable->register_callback(callback);
}

void bps_chan_ctrl::remove_enable_callback(const std::function<void()> &callback)
{
    _enable->remove_callback(callback);
}
