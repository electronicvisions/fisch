#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(I2CINA219RoRegister, 0, 0x1234)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CINA219RoRegister, 0x1234)

TEST(I2CINA219RoRegister, EncodeRead)
{
	using namespace hxcomm::vx;

	typename I2CINA219RoRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CINA219RoRegisterOnINA219::bus,
	    halco::hicann_dls::vx::INA219OnBoard::vdd12_analog);
	auto messages = I2CINA219RoRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 4);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2), false));

	auto addr_read = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2), true));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ina219_base_address + 2 + i2c_over_omnibus_stop), true));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x2)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_read);
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(message_addr_3, addr_read_stop);
}

TEST(I2CINA219RoRegister, Ostream)
{
	I2CINA219RoRegister obj;
	obj.set(I2CINA219RoRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CINA219RoRegister(0d13 0xd 0b0000000000001101)");
}
