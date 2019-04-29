#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"

TEST(OmnibusChip, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(OmnibusChip());

	OmnibusChip default_config;
	EXPECT_EQ(default_config.get(), OmnibusChip::value_type());

	OmnibusChip::value_type value(0x12345678);
	OmnibusChip value_config(value);
	EXPECT_EQ(value_config.get(), value);

	OmnibusChip::value_type other_value(0x87654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	OmnibusChip other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(OmnibusChip, Ostream)
{
	using namespace fisch::vx;

	OmnibusChip obj(OmnibusData(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "OmnibusChip(0d13 0xd 0b00000000000000000000000000001101)");
}

TEST(OmnibusChip, CerealizeCoverage)
{
	using namespace fisch::vx;

	OmnibusChip obj1, obj2;
	obj1.set(OmnibusChip::value_type(0x12345678));

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
