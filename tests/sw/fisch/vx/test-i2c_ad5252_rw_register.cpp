#include <gtest/gtest.h>

#include "fisch/cerealization.tcc"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "halco/hicann-dls/vx/ultra96.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(I2CAD5252RwRegister, 0, 0x70)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CAD5252RwRegister, 0x70)

TEST(I2CAD5252RwRegister, EncodeRead)
{
	using namespace hxcomm::vx;
	using namespace halco::hicann_dls::vx;

	typename I2CAD5252RwRegister::coordinate_type coord(
	    I2CAD5252RwRegisterOnAD5252Channel::rdac_volatile, AD5252ChannelOnBoard::vdd_12madc);
	auto messages = I2CAD5252RwRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 3);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ad5252_base_address + 0x01), false));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ad5252_base_address + 0x01) | i2c_over_omnibus_stop, true));

	auto data_register_address = UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	    instruction::omnibus_to_fpga::Data::Payload(
	        I2CAD5252RwRegisterOnAD5252Channel::rdac_volatile_prefix + 0x03));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(message_data_1, data_register_address);
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_read_stop);
}

TEST(I2CAD5252RwRegister, EncodeWrite)
{
	using namespace hxcomm::vx;
	using namespace halco::hicann_dls::vx;

	typename I2CAD5252RwRegister::coordinate_type coord(
	    I2CAD5252RwRegisterOnAD5252Channel::rdac_volatile, AD5252ChannelOnBoard::vdd_12madc);
	I2CAD5252RwRegister reg(I2CAD5252RwRegister::Value(0x70));
	auto messages = reg.encode_write(coord);

	EXPECT_EQ(messages.size(), 4);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_ad5252_base_address + 0x01));

	auto addr_write_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ad5252_base_address + 0x01) | i2c_over_omnibus_stop));

	auto data_register_address = UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	    instruction::omnibus_to_fpga::Data::Payload(
	        I2CAD5252RwRegisterOnAD5252Channel::rdac_volatile_prefix + 0x03));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(message_data_1, data_register_address);

	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_write_stop);
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x70)));
}

TEST(I2CAD5252RwRegister, Ostream)
{
	I2CAD5252RwRegister obj;
	obj.set(I2CAD5252RwRegister::Value(0x70));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CAD5252RwRegister(0d112 0x70 0b01110000)");
}
