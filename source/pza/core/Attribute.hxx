#pragma once

#include <string>
#include <pza/utils/Utils.hxx>
#include <pza/core/Field.hxx>
#include <condition_variable>

#define SET_TIMEOUT 1 // in seconds
#define SET_TIMEOUT_RETRIES 3

namespace pza
{
    class Attribute
    {
    private:
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

            if ((value1 == UNSIGNED || value1 == INTEGER || value1 == FLOAT) && (value2 == UNSIGNED || value2 == INTEGER || value2 == FLOAT))
            {
                return true;
            }
            return false;
        }

        virtual void onData(const json &json)
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

        void dataFromField(const json &data, bool ensure)
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

        void setCallback(const std::function<void(const json &data)> &callback)
        {
            for (auto &field : getFields())
            {
                field.second->setCallback(std::bind(&Attribute::dataFromField, this, std::placeholders::_1, std::placeholders::_2));
            }
            _callback = callback;
        }

        std::function<void(const json &data)> _callback;

        virtual const std::unordered_map<std::string, FieldBase *> &getFields() const = 0;

        const std::string &name() const
        {
            return _name;
        }

    protected:
        explicit Attribute(const std::string &name)
            : _name(name)
        {
        }

    private:
        void setTopic(const std::string &topic)
        {
            _topic = topic;
        }

        std::string topic() const
        {
            return _topic;
        }

        std::string _name;
        std::string _topic;
        bool _waitingForResponse = false;
        std::condition_variable _cv;
        std::mutex _mtx;
    };
};