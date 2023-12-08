#include "attribute.hxx"

attribute::attribute(const std::string &name)
    : _name(name)
{

}

attribute::~attribute()
{
}

void attribute::on_message(mqtt::const_message_ptr msg)
{
    auto json = json_attribute(_name);

    if (json.parse(msg->get_payload()) < 0) {
        spdlog::error("attribute::on_message: failed to parse payload");
        return;
    }

    for (auto &field : _fields) {
        auto &name = field.first;
        auto &type = field.second;

        if (std::holds_alternative<std::string>(type)) {
            _set_field<std::string>(json, name);
        }
        else if (std::holds_alternative<unsigned int>(type)) {
            _set_field<unsigned int>(json, name);
        }
        else if (std::holds_alternative<int>(type)) {
            _set_field<int>(json, name);
        }
        else if (std::holds_alternative<double>(type)) {
            _set_field<double>(json, name);
        }
        else if (std::holds_alternative<bool>(type)) {
            _set_field<bool>(json, name);
        }
        else {
            spdlog::error("attribute::on_message: unknown field type");
        }
    }

    _cv.notify_all();

    for (auto &cb : _callbacks) {
        cb();
    }
}

void attribute::register_callback(const std::function<void(void)> &cb)
{
    _callbacks.push_back(cb);
}

void attribute::remove_callback(const std::function<void(void)> &cb)
{
    _callbacks.remove_if([&](const std::function<void(void)> &f) {
        return f.target_type() == cb.target_type() && f.target<void(void)>() == cb.target<void(void)>();
    });
}
