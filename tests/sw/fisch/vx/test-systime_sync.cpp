#include "gtest/gtest.h"

#include "fisch/cerealization.h"
#include "fisch/vx/systime.h"

TEST(SystimeSync, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SystimeSync());
	EXPECT_NO_THROW(SystimeSync(true));

	SystimeSync config;
	SystimeSync other_config = config;

	EXPECT_EQ(other_config, config);

	bool const value = true;
	SystimeSync config2;
	config2.set(value);

	EXPECT_NE(config, config2);

	SystimeSync config3(value);

	EXPECT_NE(config, config3);
	EXPECT_EQ(config2, config3);

	auto encoded = config2.encode_write(SystimeSync::coordinate_type());
	using namespace hxcomm::vx;
	EXPECT_EQ(
	    boost::get<UTMessageToFPGA<instruction::timing::SystimeInit>>(encoded.front()).decode(),
	    true);
}

TEST(SystimeSync, Ostream)
{
	using namespace fisch::vx;

	SystimeSync obj;
	obj.set(true);

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SystimeSync(true)");
}

TEST(SystimeSync, CerealizeCoverage)
{
	using namespace fisch::vx;

	SystimeSync obj1, obj2;
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
