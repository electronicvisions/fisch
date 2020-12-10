#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(I2CIdRegister, 0, 0x12345678)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CIdRegister, 0x12345678)

TEST(I2CIdRegister, EncodeRead)
{
	using namespace hxcomm::vx;

	typename I2CIdRegister::coordinate_type coord;
	auto messages = I2CIdRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 6);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_eeprom_base_address, false));

	auto addr_read = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_eeprom_base_address, true));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_eeprom_base_address | i2c_over_omnibus_stop, true));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0xfc)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_read);
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(message_addr_3, addr_read);
	auto message_addr_4 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_4, addr_read);
	auto message_addr_5 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(5));
	EXPECT_EQ(message_addr_5, addr_read_stop);
}

TEST(I2CIdRegister, Ostream)
{
	I2CIdRegister obj;
	obj.set(I2CIdRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CIdRegister(0d13 0xd 0b00000000000000000000000000001101)");
}
