#pragma once

#include <iostream>
#include <unordered_map>

#include <pza/core/Topic.hxx>
#include <pza/core/Attribute.hxx>

namespace pza
{
    class Client;
    class Psu;

    class Interface
    {
        enum class State
        {
            Orphan,
            Unconnected,
            Running
        };

        friend class Client;

    public:

        enum class Type
        {
            Platform,
            Psu,
            Unknown
        };

        void bindToClient(Client *client);
        void unbind(void);

        bool isRunning(void) const
        {
            return (_state == State::Running);
        }

        const std::string &name(void) const
        {
            return _topic.interface();
        }

        Type type(void) const
        {
            return _type;
        }

        State state(void) const
        {
            return _state;
        }

        static std::string TypeToString(Type type)
        {
            switch (type) {
                case Type::Platform:
                    return "platform";
                case Type::Psu:
                    return "psu";
                default:
                    return "unknown";
            }
        }

        static Type StringToType(const std::string &type)
        {
            if (type == "platform")
                return Type::Platform;
            else if (type == "psu")
                return Type::Psu;
            return Type::Unknown;
        }

        static void CreateInterface(Client *client, const std::string &name, Interface::Type type);

    protected:
        explicit Interface(const std::string &name);
        ~Interface();

        virtual const std::vector<Attribute *> &getAttributes() const = 0;
        Type _type = Type::Unknown;

    private:
        void setState(State state)
        {
            _state = state;
        }

        void setBaseTopic(const std::string &name)
        {
            _baseTopic = name;
        }

        void dataFromAttribute(const json &data);
        void registerAttributes(void);
        void unregisterAttributes(void);
        void unsubscribe(void);
        void subscribe(void);
        void sendMessage(const std::string &payload);
        void reconnect(void);
        void disconnect(void);

        std::string _tmpName;
        State _state;
        Topic _topic;
        Client *_client = nullptr;
        std::string _baseTopic;
        std::vector<std::string> _attsTopic;
    };
};