#pragma once

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class FieldBase {
public:
    virtual const std::string &_getName() const = 0;
    virtual const std::type_info &_getType() const = 0;
    virtual const json::value_t &_getJsonType() const = 0;
    virtual void setCallback(const std::function<void(const json &data, bool ensure)> &callback) = 0;
};

template <typename T>
class Field : public FieldBase
{
    friend class Attribute;

public:

    explicit Field(const std::string &name)
        : _name(name)
    {
        _setJsonType();
    }

    const T &get(void) const
    {
        return _value;
    }

protected:

    void _setValue(const T &value)
    {
        _value = value;
    }
    
    const json::value_t &_getJsonType() const override
    {
       return jsonType;
    }

    const std::string &_getName() const override
    {
        return _name;
    }

    const std::type_info &_getType() const override
    {
        return typeid(T);
    }

    std::string _name;
    T _value = T();
    json::value_t jsonType;

private:
    virtual void setCallback(const std::function<void(const json &data, bool ensure)> &callback) override
    {
        (void)callback;
    }

    void _setJsonType()
    {
        if (typeid(T) == typeid(int)) {
            jsonType = json::value_t::number_integer;
        }
        else if (typeid(T) == typeid(double)) {
            jsonType = json::value_t::number_float;
        }
        else if (typeid(T) == typeid(bool)) {
            jsonType = json::value_t::boolean;
        }
        else if (typeid(T) == typeid(std::string)) {
            jsonType = json::value_t::string;
        }
        else {
            jsonType = json::value_t::null;
        }
    }
};

template <typename T>
class RoField : public Field<T>
{
public:
    explicit RoField(const std::string &name)
        : Field<T>(name)
    {

    };
};

template <typename T>
class RwField : public Field<T>
{
public:
    explicit RwField(const std::string &name)
        : Field<T>(name)
    {

    };

    void _set(const T &value, bool ensure)
    {
        json data;

        data[this->_name] = value;
        _callback(data, ensure);
    }

    void aset(const T &value)
    {
        _set(value, false);
    }

    void set(const T &value)
    {
        _set(value, true);
    }

private:
    void setCallback(const std::function<void(const json &data, bool ensure)> &callback) override
    {
        _callback = callback;
    }

    std::function<void(const json &data, bool ensure)> _callback = [](const json &data, bool ensure)
    {
        (void)data;
        (void)ensure;
    };
};