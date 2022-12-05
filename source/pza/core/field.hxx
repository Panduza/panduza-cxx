#pragma once

#include <list>

#include <nlohmann/json.hpp>
    
#include <spdlog/spdlog.h>

namespace pza
{
    enum class access_mode
    {
        readonly,
        readwrite
    };

    template <typename _type>
    class field
    {
    public:
        friend class attribute;

        explicit field(const std::string &name, access_mode mode = access_mode::readonly)
            : _value(_type()),
            _name(name),
            _mode(mode)
        {
            _setJsonType();
        }
        
        const std::string &name() const
        {
            return _name;
        }

        const _type &get(void) const
        {
            return _value;
        }

        int set(const _type &value)
        {
            nlohmann::json data;

            if (value == _value)
                return 0;

            if (!_callback) {
                spdlog::error("No callback set for field.. Make sure the interface is bound to a client.");
                return -1;
            }
            data[this->_name] = value;
            return _callback(data);
        }

        bool is_readonly(void) const
        {
            return (_mode == access_mode::readonly);
        }

        using get_callback_type = std::function<void(_type)>;

        void add_get_callback(const get_callback_type &callback)
        {
            _get_callbacks.push_back(std::make_shared<get_callback_type>(callback));
        }

        void remove_get_callback(const get_callback_type &callback)
        {
            _get_callbacks.remove_if([&](const std::shared_ptr<get_callback_type>& ptr) {
                return callback.target_type() == ptr->target_type();
            });
        }

    private:
        void _setJsonType()
        {
            if (typeid(_type) == typeid(int))
            {
                _json_type = nlohmann::json::value_t::number_integer;
            }
            else if (typeid(_type) == typeid(double))
            {
                _json_type = nlohmann::json::value_t::number_float;
            }
            else if (typeid(_type) == typeid(bool))
            {
                _json_type = nlohmann::json::value_t::boolean;
            }
            else if (typeid(_type) == typeid(std::string))
            {
                _json_type = nlohmann::json::value_t::string;
            }
            else if (typeid(_type) == typeid(std::nullptr_t))
            {
                _json_type = nlohmann::json::value_t::null;
            }
            else
            {
                throw std::runtime_error("Invalid type");
            }
        }

        const nlohmann::json::value_t &get_json_type() const
        {
            return _json_type;
        }

        void _set_value(const _type &value)
        {
            _value = value;
            for (auto const &cb : _get_callbacks) {
                (*cb)(value);
            }
        }

        _type _value;
        std::string _name;
        nlohmann::json::value_t _json_type;
        std::list<std::shared_ptr<get_callback_type>> _get_callbacks;
        access_mode _mode;
        std::function<int(const nlohmann::json &data)> _callback;
    };
};