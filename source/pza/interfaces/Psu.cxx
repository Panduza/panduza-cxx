#include <pza/interfaces/Psu.hxx>

using namespace pza;

Psu::Psu(Client &client, const std::string &name)
    : Interface(client, name)
{
    
}