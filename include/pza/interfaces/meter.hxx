#pragma once

#include <functional>
#include <memory>
#include <string>

#include <pza/core/interface.hxx>

namespace pza::itf
{
class meter : public itf_base
{
public:
	using s_ptr = std::shared_ptr<meter>;
	using u_ptr = std::unique_ptr<meter>;
	using w_ptr = std::weak_ptr<meter>;

	explicit meter(mqtt_service *mqtt, itf_info &info);
	meter(const meter &) = delete;
	meter(meter &&) = delete;
	meter &operator=(const meter &) = delete;
	meter &operator=(meter &&) = delete;
	~meter() override;

	double get_measure();

	void register_measure_callback(const std::function<void()> &callback);
	void remove_measure_callback(const std::function<void()> &callback);

private:
	attribute_ptr _measure;
};
};
