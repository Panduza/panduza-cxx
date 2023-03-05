#include <pza/interfaces/Psu.hxx>

using namespace pza;

Psu::Psu(const std::string &name)
    : Interface(name)
{
    _type = Interface::Type::Psu;
}