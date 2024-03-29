#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/jtag.h"
#include "hxcomm/vx/utmessage.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

TEST(ResetJTAGTap, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(ResetJTAGTap());

	ResetJTAGTap config;
	ResetJTAGTap other_config = config;

	EXPECT_EQ(other_config, config);
}

TEST(ResetJTAGTap, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	ResetJTAGTap obj;

	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(typename ResetJTAGTap::coordinate_type(), emplacer);

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::to_fpga_jtag::Init>>(messages.at(0));
	EXPECT_EQ(message, UTMessageToFPGA<instruction::to_fpga_jtag::Init>());
}

TEST(ResetJTAGTap, Ostream)
{
	using namespace fisch::vx;

	ResetJTAGTap obj;

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "ResetJTAGTap()");
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
