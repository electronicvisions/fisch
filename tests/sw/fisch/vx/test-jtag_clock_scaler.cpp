#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/jtag.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(JTAGClockScaler, 0, 256, 12)

TEST(JTAGClockScaler, Ostream)
{
	JTAGClockScaler obj;
	obj.set(JTAGClockScaler::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGClockScaler(12)");
}

TEST(JTAGClockScaler, EncodeWrite)
{
	using namespace hxcomm::vx;

	JTAGClockScaler obj;
	obj.set(JTAGClockScaler::Value(12));

	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(typename JTAGClockScaler::coordinate_type(), emplacer);

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::to_fpga_jtag::Scaler>>(messages.at(0));
	EXPECT_EQ(message.decode().value(), obj.get());
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(JTAGClockScaler, 12)
