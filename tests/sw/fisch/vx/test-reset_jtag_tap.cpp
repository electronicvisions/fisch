#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(ResetJTAGTap, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(ResetJTAGTap());

	ResetJTAGTap config;
	ResetJTAGTap other_config = config;

	EXPECT_EQ(other_config, config);
}

TEST(ResetJTAGTap, CerealizeCoverage)
{
	using namespace fisch::vx;

	ResetJTAGTap obj1, obj2;

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
