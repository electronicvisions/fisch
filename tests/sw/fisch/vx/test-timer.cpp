#include <gtest/gtest.h>

#include "fisch/vx/timer.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/timing.h"
#include "hxcomm/vx/utmessage.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

TEST(Timer, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(Timer());

	Timer config;
	Timer other_config = config;

	EXPECT_EQ(other_config, config);
}

TEST(Timer, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Timer obj;

	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(typename Timer::coordinate_type(), emplacer);

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::timing::Setup>>(messages.at(0));
	EXPECT_EQ(message, UTMessageToFPGA<instruction::timing::Setup>());
}

TEST(Timer, Ostream)
{
	using namespace fisch::vx;

	Timer obj;

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "Timer(0)");
}

TEST(Timer, CerealizeCoverage)
{
	using namespace fisch::vx;

	Timer obj1, obj2;

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
