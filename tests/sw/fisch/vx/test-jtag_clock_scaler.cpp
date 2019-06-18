#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

TEST(JTAGClockScaler, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(JTAGClockScaler::Value());
	EXPECT_EQ(JTAGClockScaler::Value().value(), 0);
	EXPECT_THROW(JTAGClockScaler::Value(256), std::overflow_error);

	EXPECT_NO_THROW(JTAGClockScaler());

	JTAGClockScaler config;
	EXPECT_EQ(config.get(), JTAGClockScaler::Value());

	JTAGClockScaler::Value const value(12);
	config.set(value);
	EXPECT_EQ(config.get(), value);

	JTAGClockScaler config2(value);
	EXPECT_EQ(config2.get(), value);

	JTAGClockScaler other_config = config;

	EXPECT_EQ(other_config, config);
	EXPECT_NE(JTAGClockScaler(), config);
}

TEST(JTAGClockScaler, Ostream)
{
	using namespace fisch::vx;

	JTAGClockScaler obj;
	obj.set(JTAGClockScaler::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGClockScaler(12)");
}

TEST(JTAGClockScaler, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	JTAGClockScaler obj;
	obj.set(JTAGClockScaler::Value(12));

	auto messages = obj.encode_write(typename JTAGClockScaler::coordinate_type());

	EXPECT_EQ(messages.size(), 1);
	auto message = boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Scaler>>(messages.at(0));
	EXPECT_EQ(message.decode().value(), obj.get());
}

TEST(JTAGClockScaler, CerealizeCoverage)
{
	using namespace fisch::vx;

	JTAGClockScaler obj1, obj2;
	obj1.set(JTAGClockScaler::Value(12));

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
