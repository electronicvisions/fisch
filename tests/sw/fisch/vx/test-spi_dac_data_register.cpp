#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/spi.h"

TEST(SPIDACDataRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SPIDACDataRegister());

	SPIDACDataRegister default_config;
	EXPECT_EQ(default_config.get(), SPIDACDataRegister::Value());

	SPIDACDataRegister::Value value(0x678);
	SPIDACDataRegister value_config(value);
	EXPECT_EQ(value_config.get(), value);

	SPIDACDataRegister::Value other_value(0x321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	SPIDACDataRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(SPIDACDataRegister, Ostream)
{
	using namespace fisch::vx;

	SPIDACDataRegister obj(SPIDACDataRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIDACDataRegister(0d13 0xd 0b000000001101)");
}

TEST(SPIDACDataRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	SPIDACDataRegister obj1, obj2;
	obj1.set(SPIDACDataRegister::Value(0x678));

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
