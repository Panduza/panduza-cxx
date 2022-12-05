#pragma once

#include "interface.hxx"

#include <pza/utils/string.hxx>

#include <memory>
#include <vector>

namespace pza
{
    class device;

    class grouped_interface
    {
    public:
        using ptr = std::shared_ptr<grouped_interface>;

        grouped_interface() = delete;
        grouped_interface(const grouped_interface&) = delete;
        grouped_interface(grouped_interface&&) = delete;
        ~grouped_interface() = delete;

        template <typename T>
        static int register_interfaces(device *device, const std::string &name, const std::map<std::string, std::string> &map, std::vector<std::shared_ptr<T>> &channels)
        {
            int ret = 0;
            size_t pos = 0;
            int chan_id = -1;

            for (auto const &elem : map) {
                if (pza::string::starts_with(elem.first, ":" + name + "_") == true) {
                    pos = elem.first.find_first_of('_') + 1;
                    chan_id = std::stoi(elem.first.substr(pos, elem.first.find_last_of(':') - pos));
                }
            }

            if (chan_id == -1) {
                spdlog::error("No {} channels found", name);
                return -1;
            }

            channels.reserve(chan_id + 1);
            for (int i = 0; i < chan_id + 1; i++) {
                channels.push_back(std::make_shared<T>(device, ":" + name + "_" + std::to_string(i) + ":"));
            }

            return ret;
        }
    };
};