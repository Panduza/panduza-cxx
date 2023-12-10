#include "json_attribute.hxx"

json_attribute::json_attribute(std::string attribute)
    : _attribute(std::move(attribute))
{
}

int json_attribute::parse(const std::string &payload)
{
	try {
		_json = nlohmann::json::parse(payload);
		_json = _json[_attribute];
	} catch (nlohmann::json::parse_error &e) {
		return -1;
	}
	return 0;
}

int json_attribute::get_string(const std::string &key, std::string &str) { return get(key, str); }

int json_attribute::get_int(const std::string &key, int &i) { return get(key, i); }

int json_attribute::get_unsigned_int(const std::string &key, unsigned &u) { return get(key, u); }

int json_attribute::get_double(const std::string &key, double &f) { return get(key, f); }

int json_attribute::get_bool(const std::string &key, bool &b) { return get(key, b); }
