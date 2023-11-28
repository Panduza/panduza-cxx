#include <pza/interfaces/bps_chan_ctrl.hxx>

#include "../core/attribute.hxx"

using namespace pza::itf;

bps_chan_ctrl::bps_chan_ctrl(mqtt_service &mqtt, itf_info &info)
    : itf_base(mqtt, info)
{
    _att_voltage = std::make_unique<attribute>("voltage");
    _att_current = std::make_unique<attribute>("current");
    _enable = std::make_unique<attribute>("enable");

    _att_voltage->add_rw_field<double>("value");
    _att_voltage->add_ro_field<double>("min");
    _att_voltage->add_ro_field<double>("max");
    _att_voltage->add_ro_field<int>("decimals");

    _att_current->add_rw_field<double>("value");
    _att_current->add_ro_field<double>("min");
    _att_current->add_ro_field<double>("max");
    _att_current->add_ro_field<int>("decimals");

    _enable->add_rw_field<bool>("value");
    _enable->add_rw_field<int>("polling_cycle");

    register_attributes({*_att_voltage, *_att_current, *_enable});
}

bps_chan_ctrl::~bps_chan_ctrl()
{

}

int bps_chan_ctrl::set_voltage(double volts)
{
    double min = _att_voltage->get_field<double>("min").get();
    double max = _att_voltage->get_field<double>("max").get();

    if (volts < min || volts > max) {
        spdlog::error("You can't set voltage to {}, range is {} to {}", volts, min, max);
        return -1;
    }

    return _att_voltage->get_field<double>("value").set(volts);
}

int bps_chan_ctrl::set_current(double amps)
{
    double min = _att_current->get_field<double>("min").get();
    double max = _att_current->get_field<double>("max").get();

    if (amps < min || amps > max) {
        spdlog::error("You can't set current to {}, range is {} to {}", amps, min, max);
        return -1;
    }

    return _att_current->get_field<double>("value").set(amps);
}

int bps_chan_ctrl::set_enable(bool enable)
{
    return _enable->get_field<bool>("value").set(enable);
}

bool bps_chan_ctrl::get_enable()
{
    return _enable->get_field<bool>("value").get();
}

int bps_chan_ctrl::set_enable_polling_cycle(double seconds)
{
    return _enable->get_field<int>("polling_cycle").set(seconds);
}

void bps_chan_ctrl::add_enable_callback(const std::function<void(bool)> &callback)
{
    _enable->get_field<bool>("value").add_get_callback(callback);
}

void bps_chan_ctrl::remove_enable_callback(const std::function<void(bool)> &callback)
{
    _enable->get_field<bool>("value").remove_get_callback(callback);
}
