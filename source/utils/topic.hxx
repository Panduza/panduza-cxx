#pragma once

#include <array>
#include <regex>
#include <string>
#include <sstream>

#include "../core/core.hxx"

class topic
{
public:
    explicit topic(const std::string &topic);

    topic(const topic&) = delete;
    topic(topic&&) = delete;
    topic& operator=(const topic&) = delete;
    topic& operator=(topic&&) = delete;

    bool is_valid() const { return _is_valid; }

    std::string get_topic() const { return _topic; }
    std::string get_group() const { return _list[1]; }
    std::string get_device_name() const { return _list[2]; }

    static std::string regexify_topic(const std::string &topic);
    static bool topic_matches(const std::string &str, const std::string &fnmatchPattern);
    static std::string _convertPattern(const std::string &fnmatchPattern);

private:
    static constexpr unsigned int _max_depth = 3;

    std::string _topic;
    bool _is_valid = false;
    std::array<std::string, _max_depth> _list;
};
