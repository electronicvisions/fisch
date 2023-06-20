#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/jtag.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(JTAGPhyRegister, 0, 4194304, 12)

TEST(JTAGPhyRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	JTAGPhyRegister obj;
	obj.set(JTAGPhyRegister::Value(12));

	typename JTAGPhyRegister::coordinate_type coord(3);
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(coord, emplacer);

	EXPECT_EQ(messages.size(), 2);
	auto message_ins = std::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins.decode(), 92);
	auto message_data = std::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
	EXPECT_EQ(message_data.decode().get_payload(), 12);
	EXPECT_EQ(message_data.decode().get_keep_response(), false);
	EXPECT_EQ(message_data.decode().get_num_bits(), 22);
}

TEST(JTAGPhyRegister, Ostream)
{
	JTAGPhyRegister obj;
	obj.set(JTAGPhyRegister::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGPhyRegister(0d12 0xc 0b0000000000000000001100)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(JTAGPhyRegister, 12)
