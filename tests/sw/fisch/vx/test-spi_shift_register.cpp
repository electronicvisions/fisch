#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/spi.h"

TEST(SPIShiftRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SPIShiftRegister());

	SPIShiftRegister default_config;
	EXPECT_EQ(default_config.get(), SPIShiftRegister::Value());

	SPIShiftRegister::Value value(0x345678);
	SPIShiftRegister value_config(value);
	EXPECT_EQ(value_config.get(), value);

	SPIShiftRegister::Value other_value(0x654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	SPIShiftRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(SPIShiftRegister, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	SPIShiftRegister obj;
	obj.set(SPIShiftRegister::Value(0x123456));

	typename SPIShiftRegister::coordinate_type coord;
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 6);
	auto addr = ut_message_to_fpga<instruction::omnibus_to_fpga::address>(
	    instruction::omnibus_to_fpga::address::payload_type(
	        1ul | spi_over_omnibus_mask | executor_omnibus_mask, false));

	auto message_addr_1 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	                        instruction::omnibus_to_fpga::data::payload_type(0x12)));
	auto message_addr_2 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	                        instruction::omnibus_to_fpga::data::payload_type(0x34)));
	auto message_addr_3 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, addr);
	auto message_data_3 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3,
	    ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	        instruction::omnibus_to_fpga::data::payload_type(executor_omnibus_mask | 0x56)));
}

TEST(SPIShiftRegister, Ostream)
{
	using namespace fisch::vx;

	SPIShiftRegister obj(SPIShiftRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIShiftRegister(0d13 0xd 0b000000000000000000001101)");
}

TEST(SPIShiftRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	SPIShiftRegister obj1, obj2;
	obj1.set(SPIShiftRegister::Value(0x345678));

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
