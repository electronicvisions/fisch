#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(JTAGPLLRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(JTAGPLLRegister());

	JTAGPLLRegister config;
	EXPECT_EQ(config.get(), JTAGPLLRegister::Value());

	JTAGPLLRegister::Value const value(12);
	config.set(value);
	EXPECT_EQ(config.get(), value);

	JTAGPLLRegister config2(value);
	EXPECT_EQ(config2.get(), value);

	JTAGPLLRegister other_config = config;

	EXPECT_EQ(other_config, config);
	EXPECT_NE(JTAGPLLRegister(), config);
}

TEST(JTAGPLLRegister, EncodeWrite)
{
	using namespace fisch::vx;
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
	using namespace fisch::vx;

	JTAGPLLRegister obj;
	obj.set(JTAGPLLRegister::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGPLLRegister(0d12 0xc 0b00000000000000000000000000001100)");
}

TEST(JTAGPLLRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	JTAGPLLRegister obj1, obj2;
	obj1.set(JTAGPLLRegister::Value(0x12345678));

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
