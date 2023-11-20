#include "attribute.hxx"

attribute::attribute(const std::string &name)
    : _name(name)
{

}

void attribute::on_message(const mqtt::const_message_ptr &message)
{
    const std::string &payload = message->get_payload_str();
    auto json = nlohmann::json::parse(payload);

    json = json[_name];
    for (auto it = json.begin(); it != json.end(); ++it) {
        spdlog::trace("Attribute {:s} received data for field {:s} with value {:s}", _name, it.key(), it.value().dump());

        auto data = it.value();
        auto elem = _fields.find(it.key());

        if (elem == _fields.end())
            continue;

        auto &f = elem->second;
        const auto &type = f.type();

        if (type == typeid(field<double>))
            _assign_value<double>(f, data);
        else if (type == typeid(field<int>))
            _assign_value<int>(f, data);
        else if (type == typeid(field<bool>))
            _assign_value<bool>(f, data);
        else if (type == typeid(field<std::string>))
            _assign_value<std::string>(f, data);
        else {
            spdlog::warn("Type mismatch for attribute {:s}, field {:s}.. ", _name, it.key());
            return ;
        }

        _waiting_for_response = false;
        _cv.notify_one();
    }
    _is_init = true;
}

bool attribute::type_is_compatible(const nlohmann::json::value_t &value1, const nlohmann::json::value_t &value2)
{
    constexpr auto INTEGER = nlohmann::json::value_t::number_integer;
    constexpr auto UNSIGNED = nlohmann::json::value_t::number_unsigned;
    constexpr auto FLOAT = nlohmann::json::value_t::number_float;

    auto isNumber = [](const nlohmann::json::value_t &value)
    {
        return value == INTEGER || value == UNSIGNED || value == FLOAT;
    };

    return (value1 == value2) || (isNumber(value1) && isNumber(value2));
}

int attribute::_data_from_field(const nlohmann::json &data)
{
    nlohmann::json json;
    int ret = -1;

    json[_name] = data;

    if (_callback) {
        spdlog::trace("Calling callback for attribute {:s}", _name);
        spdlog::trace("Data: {}", json.dump());
        _callback(json);
    }
    
    std::unique_lock<std::mutex> lock(_mtx);
    
    for (int i = 0; i < SET_TIMEOUT_RETRIES; i++)
    {
        if (_cv.wait_for(lock, std::chrono::seconds(SET_TIMEOUT), [&]() {return !_waiting_for_response; }) == true) {
            ret = 0;
            break;
        }
        else
            spdlog::warn("Timeout while waiting for response from attribute {:s}, retrying...", _name);
    }
    return ret;
}