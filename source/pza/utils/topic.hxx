#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace pza
{
    class topic
    {
    public:
        explicit topic(const std::string &topic);

        bool is_valid() const { return _is_valid; }

        std::string get_topic() const { return _topic;}
        std::string get_group() const { return _list[1]; }
        std::string get_device() const { return _list[2]; }

    private:
        std::string _topic;
        bool _is_valid = false;
        std::vector<std::string> _list;
    };
};