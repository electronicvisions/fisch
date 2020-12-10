#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "halco/hicann-dls/vx/ultra96.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(I2CDAC6573RwRegister, 0, 0x1234, 0x03fa)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CDAC6573RwRegister, 0x03fa)

TEST(I2CDAC6573RwRegister, EncodeRead)
{
	using namespace hxcomm::vx;

	typename I2CDAC6573RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::DAC6573ChannelOnBoard::v_res_meas);
	auto messages = I2CDAC6573RwRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 4);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_dac6573_base_address, false));

	auto addr_read = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_dac6573_base_address, true));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_dac6573_base_address | i2c_over_omnibus_stop, true));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(
	            halco::hicann_dls::vx::I2CDAC6573RwRegisterOnBoard::register_base_addr +
	            (0x02 << 1))));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_read);
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(message_addr_3, addr_read_stop);
}

TEST(I2CDAC6573RwRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	typename I2CDAC6573RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::DAC6573ChannelOnBoard::v_res_meas);
	I2CDAC6573RwRegister reg(I2CDAC6573RwRegister::Value(0x03fa));
	auto messages = reg.encode_write(coord);

	EXPECT_EQ(messages.size(), 6);
	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_dac6573_base_address));

	auto stop_addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_dac6573_base_address | i2c_over_omnibus_stop));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(
	            halco::hicann_dls::vx::I2CDAC6573RwRegisterOnBoard::register_base_addr +
	            (0x02 << 1))));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0xfe)));
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, stop_addr);
	auto message_data_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x80)));
}

TEST(I2CDAC6573RwRegister, Ostream)
{
	I2CDAC6573RwRegister obj;
	obj.set(I2CDAC6573RwRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CDAC6573RwRegister(0d13 0xd 0b0000000000001101)");
}
