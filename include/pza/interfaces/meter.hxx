#pragma once

#include <memory>
#include <functional>
#include <string>

#include <pza/core/device.hxx>
#include <pza/core/interface.hxx>

class attribute;

namespace pza
{
class meter : public itf
{
public:
    using s_ptr = std::shared_ptr<meter>;
    using u_ptr = std::unique_ptr<meter>;
    using w_ptr = std::weak_ptr<meter>;
    
    meter(device &dev, const std::string &name);
    ~meter();

    double get_measure();
    int set_measure_polling_cycle(double seconds);
    void add_measure_callback(const std::function<void(double)> &callback);
    void remove_measure_callback(const std::function<void(double)> &callback);

private:
    attribute *_measure;
};
};