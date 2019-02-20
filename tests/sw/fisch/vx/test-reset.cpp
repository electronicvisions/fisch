#include "gtest/gtest.h"

#include "fisch/cerealization.h"
#include "fisch/vx/reset.h"

TEST(ResetChip, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(ResetChip());
	EXPECT_NO_THROW(ResetChip(true));

	ResetChip config;
	ResetChip other_config = config;

	EXPECT_EQ(other_config, config);

	bool const value = true;
	ResetChip config2;
	config2.set(value);

	EXPECT_NE(config, config2);

	ResetChip config3(value);

	EXPECT_NE(config, config3);
	EXPECT_EQ(config2, config3);

	auto encoded = config2.encode_write(ResetChip::coordinate_type());
	using namespace hxcomm::vx;
	EXPECT_EQ(
	    boost::get<ut_message_to_fpga<instruction::system::reset>>(encoded.front()).decode(), true);
}

TEST(ResetChip, CerealizeCoverage)
{
	using namespace fisch::vx;

	ResetChip obj1, obj2;
	obj1.set(true);

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
