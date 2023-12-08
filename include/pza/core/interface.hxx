#pragma once

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>

#include <nlohmann/json.hpp>

struct itf_impl;
class mqtt_service;
class attribute;
struct itf_info;

namespace pza
{
// We can't use the name interface because it's a reserved keyford for Windows
// C++ (lol)
class itf_base
{
public:
	using s_ptr = std::shared_ptr<itf_base>;
	using u_ptr = std::unique_ptr<itf_base>;
	using w_ptr = std::weak_ptr<itf_base>;

	using attribute_ptr = std::shared_ptr<attribute>;

	virtual ~itf_base();
	itf_base(const itf_base &) = delete;
	itf_base &operator=(const itf_base &) = delete;
	itf_base(itf_base &&) = delete;
	itf_base &operator=(itf_base &&) = delete;

	[[nodiscard]] const std::string &get_name() const;
	[[nodiscard]] const std::string &get_type() const;

protected:
	explicit itf_base(mqtt_service &mqtt, itf_info &info);

	std::unique_ptr<itf_impl> _impl;
};
};
