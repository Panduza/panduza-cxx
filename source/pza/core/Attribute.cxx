#include <pza/core/Attribute.hxx>

#include <iostream>

using namespace pza;

Attribute::Attribute(const std::string &name)
    : _name(name)
{
    
}

void Attribute::onMessage(const std::string &topic, const std::string &payload)
{
    json data;

    spdlog::trace("Attribute {:s} received message on topic {:s}, data : {:s}", _name, topic, payload);

    if (utils::json::ParseJson(payload, data) == -1)
        return ;
    onData(data[_name]);
}

bool Attribute::isTypeCompatible(const json::value_t &value1, const json::value_t &value2)
{
    constexpr auto INTEGER = json::value_t::number_integer;
    constexpr auto UNSIGNED = json::value_t::number_unsigned;
    constexpr auto FLOAT = json::value_t::number_float;
    auto isNumber = [](const json::value_t &value)
    {
        return value == INTEGER || value == UNSIGNED || value == FLOAT;
    };
    return (value1 == value2) || (isNumber(value1) && isNumber(value2));
}

void Attribute::onData(const json &json)
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (auto it = json.begin(); it != json.end(); ++it)
    {
        auto field = getFields().find(it.key());
        if (field != getFields().end())
        {
            if (isTypeCompatible(it.value().type(), field->second->_getJsonType()) == true)
            {
                spdlog::trace("Setting attribute {:s} field {:s} to value {:s}", _name, it.key(), it.value().dump());
                if (field->second->_getType() == typeid(double))
                {
                    Field<double> *f = static_cast<Field<double> *>(field->second);
                    f->_setValue(it.value());
                }
                else if (field->second->_getType() == typeid(int))
                {
                    Field<int> *f = static_cast<Field<int> *>(field->second);
                    f->_setValue(it.value());
                }
                else if (field->second->_getType() == typeid(bool))
                {
                    Field<bool> *f = static_cast<Field<bool> *>(field->second);
                    f->_setValue(it.value());
                }
                else if (field->second->_getType() == typeid(std::string))
                {
                    Field<std::string> *f = static_cast<Field<std::string> *>(field->second);
                    f->_setValue(it.value());
                }
            }
            else
            {
                spdlog::warn("Type mismatch for attribute {:s}, field {:s}.. Expected {:s}, got {:s}", _name, it.key(), utils::json::GetTypeName(field->second->_getJsonType()), it.value().type_name());
            }
        }
    }
    _waitingForResponse = false;
    _cv.notify_all();
}

void Attribute::dataFromField(const json &data, bool ensure)
{
    json json;
    json[_name] = data;
    if (ensure)
        _waitingForResponse = true;
    if (_callback)
        _callback(json);
    else {
        spdlog::error("No callback set for attribute.. Make sure the interface is bound to a client.");
        return ;
    }
    if (ensure)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        for (int i = 0; i < SET_TIMEOUT_RETRIES; i++)
        {
            if (_cv.wait_for(lock, std::chrono::seconds(SET_TIMEOUT), [&]()
                    { return !_waitingForResponse; }) == true)
                break;
            else
                spdlog::warn("Timeout while waiting for response from attribute {:s}, retrying...", _name);
        }
    }
}

void Attribute::setCallback(const std::function<void(const json &data)> &callback)
{
    for (auto &field : getFields())
    {
        field.second->setCallback(std::bind(&Attribute::dataFromField, this, std::placeholders::_1, std::placeholders::_2));
    }
    _callback = callback;
}