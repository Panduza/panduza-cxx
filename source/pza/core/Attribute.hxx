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
    protected:
        explicit Attribute(const std::string &name);

    private:
        friend class Interface;

        void onMessage(const std::string &topic, const std::string &payload);
        void sendMessage(const std::string &payload);
        static bool isTypeCompatible(const json::value_t &value1, const json::value_t &value2);
        virtual void onData(const json &json);
        void dataFromField(const json &data, bool ensure);
        void setCallback(const std::function<void(const json &data)> &callback);
        virtual const std::unordered_map<std::string, FieldBase *> &getFields() const = 0;

        const std::string &name() const
        {
            return _name;
        }

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
        std::function<void(const json &data)> _callback;
    };
};
