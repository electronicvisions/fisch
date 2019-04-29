#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(OmnibusChipOverJTAG, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(OmnibusChipOverJTAG());

	OmnibusChipOverJTAG default_config;
	EXPECT_EQ(default_config.get(), OmnibusChipOverJTAG::value_type());

	OmnibusChipOverJTAG::value_type value(0x12345678);
	OmnibusChipOverJTAG value_config(value);
	EXPECT_EQ(value_config.get(), value);

	OmnibusChipOverJTAG::value_type other_value(0x87654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	OmnibusChipOverJTAG other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(OmnibusChipOverJTAG, Ostream)
{
	using namespace fisch::vx;

	OmnibusChipOverJTAG obj(OmnibusData(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "OmnibusChipOverJTAG(0d13 0xd 0b00000000000000000000000000001101)");
}

TEST(OmnibusChipOverJTAG, CerealizeCoverage)
{
	using namespace fisch::vx;

	OmnibusChipOverJTAG obj1, obj2;
	obj1.set(OmnibusChipOverJTAG::value_type(0x12345678));

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
