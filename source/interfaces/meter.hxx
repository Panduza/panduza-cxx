#pragma once

#include "../core/interface.hxx"

class meter : public itf
{
public:
    using ptr = std::shared_ptr<meter>;
    
    meter(device *device, const std::string &name);

    double get_measure();
    int set_measure_polling_cycle(double seconds);
    void add_measure_callback(const std::function<void(double)> &callback);
    void remove_measure_callback(const std::function<void(double)> &callback);

private:
    attribute _measure;
};