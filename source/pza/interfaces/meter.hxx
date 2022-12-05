#pragma once

#include <pza/core/interface.hxx>

namespace pza {
    class meter : public interface
    {
    public:
        meter(device *device, const std::string &name);

        double get_measure();
        int set_measure_polling_cycle(double seconds);

        void add_measure_callback(const std::function<void(double)> &callback);
        void remove_measure_callback(const std::function<void(double)> &callback);

    private:
        attribute _measure;
    };
};