#pragma once

#include <string>
#include <mutex>

#include <pza/core/attribute.hxx>

namespace pza
{
    class device;

    class itface
    {
    public:
        friend class device;

        itface(device *device, const std::string &name);

        void register_attribute(attribute &attribute);
        void register_attributes(const std::vector<attribute*> &list);
    
    protected:
        device *_device;
        std::string _name;
        std::string _topic_base;
        std::string _topic_cmd;

        std::map<std::string, attribute*> _attributes;

    private:
        std::mutex _mtx;
    };
};