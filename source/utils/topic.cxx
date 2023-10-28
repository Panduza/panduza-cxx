#include "topic.hxx"

topic::topic(const std::string &topic)
    : _topic(topic),
    _is_valid(false)
{
    std::stringstream strs(topic);
    std::string buf;
    std::array<std::string, _max_depth> tmp_list;
    
    for (unsigned int i = 0; i < _max_depth; i++) {
        if (!std::getline(strs, buf, '/')) {
            spdlog::error("topic: invalid topic: {}", topic);
            break;
        }
        tmp_list[i] = buf;
    }
    if (tmp_list[0] != "pza") {
        spdlog::error("topic: invalid topic: {}", topic);
        return;
    }
    _is_valid = true;
    _list = tmp_list;
}

std::string topic::regexify_topic(const std::string &topic)
{
    std::string t = topic;

    std::replace(t.begin(), t.end(), '+', '*');
    std::replace(t.begin(), t.end(), '#', '*');

    return t;
}

bool topic::topic_matches(const std::string &str, const std::string &fnmatchPattern)
{
    std::string regexPattern = _convertPattern(fnmatchPattern);
    std::regex pattern(regexPattern);

    return std::regex_match(str, pattern);
}

std::string topic::_convertPattern(const std::string &fnmatchPattern) 
{
    std::string regexPattern;

    for (auto &ch : fnmatchPattern){
        if (ch == '*')
            regexPattern += ".*";
        else if (ch == '/')
            regexPattern += "\\/";
        else
            regexPattern += ch;
    }
    regexPattern = "^" + regexPattern + "$";
    return regexPattern;
}
