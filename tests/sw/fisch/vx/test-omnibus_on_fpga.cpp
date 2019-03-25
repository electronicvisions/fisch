#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"

TEST(Omnibus, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(Omnibus());

	Omnibus default_config;
	EXPECT_EQ(default_config.get(), Omnibus::value_type());

	Omnibus::value_type value(0x12345678);
	Omnibus value_config(value);
	EXPECT_EQ(value_config.get(), value);

	Omnibus::value_type other_value(0x87654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	Omnibus other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(Omnibus, Ostream)
{
	using namespace fisch::vx;

	Omnibus obj(OmnibusData(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "Omnibus(0d13 0xd 0b00000000000000000000000000001101)");
}

TEST(Omnibus, CerealizeCoverage)
{
	using namespace fisch::vx;

	Omnibus obj1, obj2;
	obj1.set(Omnibus::value_type(0x12345678));

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
