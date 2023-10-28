#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "../utils/json.hxx"

#include "interface.hxx"
#include "interface_factory.hxx"
#include "scanner.hxx"

class client;

class device
{
public:
    using ptr = std::shared_ptr<device>;

    explicit device(client *client, const std::string &group, const std::string &name);

    const std::string &get_name() { return _name; }
    const std::string &get_group() { return _group; }
    const std::string &get_model() { return _model; }
    const std::string &get_manufacturer() { return _manufacturer; }

    const std::string &get_base_topic() { return _base_topic; }
    const std::string &get_device_topic() { return _device_topic; }

    void create_interfaces(const scanner::interface_map &interfaces);

    client *get_client() { return _cli; }

private:
    client *_cli;
    std::string _name;
    std::string _group;
    std::string _model;
    std::string _manufacturer;
    std::string _family;
    std::mutex _mtx;

    std::string _base_topic;
    std::string _device_topic;

    std::unordered_map<std::string, itf::ptr> _interfaces;

    int _register_interface(itf::ptr interface);
};
