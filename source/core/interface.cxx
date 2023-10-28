#include "interface.hxx"
#include "device.hxx"
#include "client.hxx"

itf::itf(device *device, const std::string &name)
    : _dev(device),
    _name(name)
{
    _topic_base = _dev->get_base_topic() + "/" + _name;
    _topic_cmd = _topic_base + "/cmds/set";
}

void itf::add_attributes(const std::vector<attribute *> &attributes)
{
    for (auto &it : attributes) {
        _attributes[it->get_name()] = it;
        it->set_callback(std::bind(&itf::on_new_message_ready, this, std::placeholders::_1));
    }
}

void itf::on_new_message_ready(const nlohmann::json &data)
{
    _dev->get_client()->publish(_topic_cmd, data.dump());
}