#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(JTAGPhyRegister, 0, 4194304, 12)

TEST(JTAGPhyRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	JTAGPhyRegister obj;
	obj.set(JTAGPhyRegister::Value(12));

	typename JTAGPhyRegister::coordinate_type coord(3);
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 2);
	auto message_ins = boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins.decode().value(), 92);
	auto message_data =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
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
