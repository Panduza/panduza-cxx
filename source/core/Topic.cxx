#include <core/Topic.hxx>

void Topic::setup(const std::string &name)
{
    std::stringstream strs(name);
    std::string buf;

    for (unsigned int i = 0; std::getline(strs, buf, '/'); i++) {
        if (i != 0)
            _list[i - 1] = buf;
    }
}