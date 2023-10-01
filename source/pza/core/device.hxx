#pragma once

#include <memory>
#include <string>
#include <map>
#include <iostream>

#include <pza/utils/json.hxx>

#include <pza/core/interface.hxx>

namespace pza
{
	class client;
    
	class device
    {
    public:
	    using ptr = std::shared_ptr<device>;
        
        friend class client;
        friend class itface;

        enum class state : unsigned int
        {
            orphan = 0,
            init,
            running
        };

        const std::string &get_name() { return _name; }
        const std::string &get_group() { return _group; }
	    const std::string &get_model() { return _model; }
	    const std::string &get_manufacturer() { return _manufacturer; }
	    client *get_client() { return _cli; }
		virtual const std::string &get_family() = 0;

        void reset();
        enum state get_state() { return _state; }
        void register_interface(itface &itface);

    protected:
	    device(const std::string &group, const std::string &name);

        virtual int _register_interfaces(const std::map<std::string, std::string> &map) = 0;
        int _set_identity(const std::string &payload);
	    const std::string &_get_base_topic() { return _base_topic; }
        const std::string &_get_device_topic() { return _device_topic; }

        
        client *_cli = nullptr;
       
        std::string _name;
        std::string _group;
        std::string _model = "none";
        std::string _manufacturer = "none";
        
        std::string _base_topic;
        std::string _device_topic;

        std::map<std::string, itface*> _interfaces;

        enum state _state = state::orphan;
    };
};
