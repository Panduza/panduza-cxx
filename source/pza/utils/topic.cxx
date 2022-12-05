#include "topic.hxx"

namespace pza
{
    topic::topic(const std::string &topic)
        : _topic(topic),
        _is_valid(false)
    {
        std::stringstream strs(topic);
        std::string buf;
        
        _list.resize(3);
        for (unsigned int i = 0; std::getline(strs, buf, '/') && i < 3; i++) {
            _list[i] = buf;
        }
        if (_list[0].empty() || _list[1].empty() || _list[2].empty()) {
            return ;
        }
        if (_list[0] != "pza") {
            return ;
        }
        _is_valid = true;
    }
};