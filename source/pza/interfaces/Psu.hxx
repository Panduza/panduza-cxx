#pragma once

#include <pza/core/Interface.hxx>
#include <pza/core/Attribute.hxx>
#include <pza/core/Field.hxx>

namespace pza
{
    using namespace pza;

    class PZA_DllExport AUnit : public Attribute
    {
    public:
        explicit AUnit(const std::string &name)
            : Attribute(name)
        {
        }

        const std::unordered_map<std::string, FieldBase *> &getFields() const override
        {
            return _fields;
        }

        std::unordered_map<std::string, FieldBase *> _fields = {
            {"real", &real},
            {"goal", &goal},
            {"min", &min},
            {"max", &max},
            {"decimals", &decimals}};

        RoField<double> real{"real"};
        RwField<double> goal{"goal"};
        RoField<double> min{"min"};
        RoField<double> max{"max"};
        RoField<int> decimals{"decimals"};
    };

    class PZA_DllExport AEnable : public Attribute
    {
    public:
        AEnable()
            : Attribute("enable")
        {
        }

        const std::unordered_map<std::string, FieldBase *> &getFields() const override
        {
            return _fields;
        }

        std::unordered_map<std::string, FieldBase *> _fields = {
            {"value", &value},
        };

        RwField<bool> value{"value"};
    };

    class PZA_DllExport AVolts : public AUnit
    {
    public:
        AVolts()
            : AUnit("volts")
        {
        }
    };

    class PZA_DllExport AAmps : public AUnit
    {
    public:
        AAmps()
            : AUnit("amps")
        {
        }
    };

    class PZA_DllExport ASettings : public Attribute
    {
    public:
        ASettings()
            : Attribute("settings")
        {
        }

        const std::unordered_map<std::string, FieldBase *> &getFields() const override
        {
            return _fields;
        }

        std::unordered_map<std::string, FieldBase *> _fields = {
            {"ovp", &ovp},
            {"ocp", &ocp},
            {"silent", &silent}
        };

        RwField<bool> ovp{"ovp"};
        RwField<bool> ocp{"ocp"};
        RwField<bool> silent{"silent"};
    };

    class PZA_DllExport Psu : public Interface
    {
    public:
        explicit Psu(const std::string &name);

        Type type(void) const override
        {
            return Type::Psu;
        }

        const std::vector<Attribute *> &getAttributes() const override
        {
            return _attributes;
        }

        const std::vector<Attribute *> _attributes = {
            &enable,
            &volts,
            &amps,
            &settings
        };

        AEnable enable;
        AVolts volts;
        AAmps amps;
        ASettings settings;
    };
};