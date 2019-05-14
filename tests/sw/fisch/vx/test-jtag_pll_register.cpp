#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(JTAGPLLRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(JTAGPLLRegister());

	JTAGPLLRegister config;

	JTAGPLLRegister config2;
	config2.set(JTAGPLLRegister::Value());
	EXPECT_EQ(config, config2);

	JTAGPLLRegister::Value data(0x12345678);
	JTAGPLLRegister config3;
	config3.set(data);
	EXPECT_NE(config, config3);

	JTAGPLLRegister config4(data);
	EXPECT_NE(config, config4);
	EXPECT_EQ(config3, config4);

	auto encoded = config3.encode_write(JTAGPLLRegister::coordinate_type());
	EXPECT_EQ(encoded.size(), JTAGPLLRegister::encode_write_ut_message_count);

	using namespace hxcomm::vx;
	auto payload = boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::data>>(encoded.back())
	                   .decode()
	                   .get_payload();
	EXPECT_EQ(payload, data.value());
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
	    boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::ins>>(messages.at(0));
	EXPECT_EQ(message_ins_1.decode().value(), instruction::to_fpga_jtag::ins::PLL_TARGET_REG);
	auto message_data_1 =
	    boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::data>>(messages.at(1));
	EXPECT_EQ(message_data_1.decode().get_payload(), coord.toEnum().value());
	EXPECT_EQ(message_data_1.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_1.decode().get_num_bits(), 3);
	auto message_ins_2 =
	    boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::ins>>(messages.at(2));
	EXPECT_EQ(message_ins_2.decode().value(), instruction::to_fpga_jtag::ins::SHIFT_PLL);
	auto message_data_2 =
	    boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::data>>(messages.at(3));
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
