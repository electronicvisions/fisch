#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/spi.h"

TEST(SPIDACControlRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SPIDACControlRegister());

	SPIDACControlRegister default_config;
	EXPECT_EQ(default_config.get(), SPIDACControlRegister::Value());

	SPIDACControlRegister::Value value(0x678);
	SPIDACControlRegister value_config(value);
	EXPECT_EQ(value_config.get(), value);

	SPIDACControlRegister::Value other_value(0x321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	SPIDACControlRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(SPIDACControlRegister, Ostream)
{
	using namespace fisch::vx;

	SPIDACControlRegister obj(SPIDACControlRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIDACControlRegister(0d13 0xd 0b0000000001101)");
}

TEST(SPIDACControlRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	SPIDACControlRegister obj1, obj2;
	obj1.set(SPIDACControlRegister::Value(0x678));

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
