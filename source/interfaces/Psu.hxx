#pragma once

#include <core/Interface.hxx>
#include <core/Attribute.hxx>
#include <core/Field.hxx>

#define ADD_FIELD(name)       \
    class name : public Field \
    {                         \
    public:                   \
        name() : Field(#name) \
        {                     \
        }                     \
    } name;

#define ADD_ATTRIBUTE(name, ...)        \
    class name : public Attribute{      \
        public :                        \
            name() : Attribute(#name){} \
                                        \
        __VA_ARGS__                     \
                                        \
    } name;

class Psu : public Interface
{
public:
    Psu(Client &client, const std::string &name);

    ADD_ATTRIBUTE(state,
        ADD_FIELD(value)
    )
    ADD_ATTRIBUTE(volts,
        ADD_FIELD(value)
        ADD_FIELD(min)
        ADD_FIELD(max)
        ADD_FIELD(decimals)
    )
    ADD_ATTRIBUTE(amps,
        ADD_FIELD(value)
        ADD_FIELD(min)
        ADD_FIELD(max)
        ADD_FIELD(decimals)
    )
    ADD_ATTRIBUTE(settings,
        ADD_FIELD(ovp)
    )
};