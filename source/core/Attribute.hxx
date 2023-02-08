#pragma once

#include <string>
#include <utils/Utils.hxx>
#include <core/Field.hxx>

class Attribute
{
public:

    friend class Interface;

    void onMessage(const std::string &topic, const std::string &payload);

    void sendMessage(const std::string &payload);

    static bool isTypeCompatible(const json::value_t &value1, const json::value_t &value2)
    {
        constexpr auto INTEGER = json::value_t::number_integer;
        constexpr auto UNSIGNED = json::value_t::number_unsigned;
        constexpr auto FLOAT = json::value_t::number_float;

        if (value1 == value2)
            return true;

        if ( (value1 == UNSIGNED || value1 == INTEGER || value1 == FLOAT)
            && (value2 == UNSIGNED || value2 == INTEGER || value2 == FLOAT)) {
            return true;
        }
        return false;
    }

    virtual void onData(const json& json) 
    {
        for (auto it = json.begin(); it != json.end(); ++it) {
            auto field = getFields().find(it.key());
            if (field != getFields().end()) {
                if (isTypeCompatible(it.value().type(), field->second->_getJsonType()) == true) {
                    if (field->second->_getType() == typeid(double)) {
                        Field<double>* f = static_cast<Field<double>*>(field->second);
                        f->_setValue(it.value());
                    }
                    else if (field->second->_getType() == typeid(int)) {
                        Field<int>* f = static_cast<Field<int>*>(field->second);
                        f->_setValue(it.value());
                    }
                    else if (field->second->_getType() == typeid(bool)) {
                        Field<bool>* f = static_cast<Field<bool>*>(field->second);
                        f->_setValue(it.value());
                    }
                    else if (field->second->_getType() == typeid(std::string)) {
                        Field<std::string>* f = static_cast<Field<std::string>*>(field->second);
                        f->_setValue(it.value());
                    }
                }
            }
        }
    }

    void dataFromField(const json &data)
    {
        json json;

        json[_name] = data;
        _callback(json);
    }

    void setCallback(const std::function<void(const json &data)> &callback)
    {
        for (auto &field : getFields()) {
            field.second->setCallback(std::bind(&Attribute::dataFromField, this, std::placeholders::_1));
        }
        _callback = callback;
    }

    std::function<void(const json &data)> _callback;

    virtual const std::unordered_map<std::string, FieldBase*> &getFields() const = 0;

    const std::string &name() const
    {
        return _name;
    }

protected:
    explicit Attribute(const std::string &name)
        : _name(name)
    {

    }

    std::string _name;
};