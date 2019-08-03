#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(JTAGPLLRegister, 0, 12)

TEST(JTAGPLLRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	JTAGPLLRegister obj;
	obj.set(JTAGPLLRegister::Value(12));

	typename JTAGPLLRegister::coordinate_type coord(3);
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 4);
	auto message_ins_1 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins_1.decode().value(), instruction::to_fpga_jtag::Ins::PLL_TARGET_REG);
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
	EXPECT_EQ(message_data_1.decode().get_payload(), coord.toEnum().value());
	EXPECT_EQ(message_data_1.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_1.decode().get_num_bits(), 3);
	auto message_ins_2 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(2));
	EXPECT_EQ(message_ins_2.decode().value(), instruction::to_fpga_jtag::Ins::SHIFT_PLL);
	auto message_data_2 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(3));
	EXPECT_EQ(message_data_2.decode().get_payload(), 12);
	EXPECT_EQ(message_data_2.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_2.decode().get_num_bits(), 32);
}

TEST(JTAGPLLRegister, Ostream)
{
	JTAGPLLRegister obj;
	obj.set(JTAGPLLRegister::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGPLLRegister(0d12 0xc 0b00000000000000000000000000001100)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(JTAGPLLRegister, 0x12345678)
