#include <gtest/gtest.h>
#include <pza/core/core.hxx>

TEST(core, log_level)
{
	pza::core::set_log_level(pza::core::log_level::trace);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::trace);

	pza::core::set_log_level(pza::core::log_level::debug);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::debug);

	pza::core::set_log_level(pza::core::log_level::info);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::info);

	pza::core::set_log_level(pza::core::log_level::warn);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::warn);

	pza::core::set_log_level(pza::core::log_level::err);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::err);

	pza::core::set_log_level(pza::core::log_level::critical);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::critical);

	pza::core::set_log_level(pza::core::log_level::off);
	EXPECT_EQ(pza::core::get_log_level(), pza::core::log_level::off);
}

TEST(core, version)
{
	EXPECT_NE(pza::core::get_version(), "");
	EXPECT_NE(pza::core::get_version_githash(), "");
}
