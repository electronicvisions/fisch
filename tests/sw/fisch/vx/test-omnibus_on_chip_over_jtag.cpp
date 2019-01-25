#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(OmnibusOnChipOverJTAG, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(OmnibusOnChipOverJTAG());

	OmnibusOnChipOverJTAG default_config;
	EXPECT_EQ(default_config.get(), OmnibusOnChipOverJTAG::value_type());

	OmnibusOnChipOverJTAG::value_type value(0x12345678);
	OmnibusOnChipOverJTAG value_config(value);
	EXPECT_EQ(value_config.get(), value);

	OmnibusOnChipOverJTAG::value_type other_value(0x87654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	OmnibusOnChipOverJTAG other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(OmnibusOnChipOverJTAG, CerealizeCoverage)
{
	using namespace fisch::vx;

	OmnibusOnChipOverJTAG obj1, obj2;
	obj1.set(OmnibusOnChipOverJTAG::value_type(0x12345678));

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
