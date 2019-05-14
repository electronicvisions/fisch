#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/timer.h"

TEST(Timer, General)
{
	using namespace fisch::vx;

	EXPECT_EQ(Timer::Value().value(), 0);

	EXPECT_NO_THROW(Timer());

	Timer config;
	EXPECT_EQ(config.get(), Timer::Value());

	EXPECT_NO_THROW(config.set(Timer::Value(0)));
	EXPECT_THROW(config.set(Timer::Value(1)), std::runtime_error);

	Timer config2(Timer::Value(0));
	EXPECT_EQ(config2, config);

	Timer other_config = config;

	EXPECT_EQ(other_config, config);
}

TEST(Timer, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Timer obj;

	auto messages = obj.encode_write(typename Timer::coordinate_type());

	EXPECT_EQ(messages.size(), 1);
	auto message = boost::get<ut_message_to_fpga<instruction::timing::setup>>(messages.at(0));
	EXPECT_EQ(message, ut_message_to_fpga<instruction::timing::setup>());
}

TEST(Timer, Ostream)
{
	using namespace fisch::vx;

	Timer obj;

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "Timer(0)");
}

TEST(Timer, CerealizeCoverage)
{
	using namespace fisch::vx;

	Timer obj1, obj2;

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}
