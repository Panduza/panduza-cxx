#pragma once

#include <mutex>
#include <string>

#include "attribute.hxx"

class device;

// We can't use the name interface because it's a reserved keyford for Windows C++ (lol)
class itf
{
public:
    using ptr = std::shared_ptr<itf>;
    using attribute_map = std::unordered_map<std::string, attribute *>;

    virtual ~itf() = default;
    itf(const itf&) = delete;
    itf& operator=(const itf&) = delete;
    itf(itf&&) = delete;
    itf& operator=(itf&&) = delete;

    const attribute_map &get_attributes() const { return _attributes; }
    const std::string &get_name() const { return _name; }
    const std::string &get_topic_base() const { return _topic_base; }
    const std::string &get_topic_cmd() const { return _topic_cmd; }

    void on_new_message_ready(const nlohmann::json &data);

protected:
    itf(device *device, const std::string &name);

    void add_attributes(const std::vector<attribute *> &attributes);

private:
    std::mutex _mtx;
    device *_dev;
    std::string _name;
    std::string _topic_base;
    std::string _topic_cmd;
    attribute_map _attributes;
};
