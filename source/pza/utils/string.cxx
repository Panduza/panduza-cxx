#include "string.hxx"

using namespace pza;

bool string::starts_with(const std::string &s, const std::string &prefix)
{
    return (s.rfind(prefix, 0) == 0);
}