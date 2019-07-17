#include "gtest/gtest.h"

#include "fisch/cerealization.h"
#include "fisch/vx/reset.h"

TEST(ResetChip, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(ResetChip());
	EXPECT_NO_THROW(ResetChip(true));

	ResetChip config;
	EXPECT_EQ(config.get(), false);

	bool const value = !config.get();
	config.set(value);
	EXPECT_EQ(config.get(), value);

	ResetChip config2(value);
	EXPECT_EQ(config2.get(), value);

	ResetChip other_config = config;

	EXPECT_EQ(other_config, config);
	EXPECT_NE(ResetChip(), config);
}

TEST(ResetChip, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	bool value = true;
	ResetChip obj(value);

	auto messages = obj.encode_write(typename ResetChip::coordinate_type());

	EXPECT_EQ(messages.size(), 1);
	auto message = boost::get<UTMessageToFPGA<instruction::system::Reset>>(messages.at(0));
	EXPECT_EQ(message.decode(), value);
}

TEST(ResetChip, Ostream)
{
	using namespace fisch::vx;

	ResetChip obj;
	obj.set(true);

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "ResetChip(true)");
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
