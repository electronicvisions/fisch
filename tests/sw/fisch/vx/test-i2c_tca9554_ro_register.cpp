#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "halco/hicann-dls/vx/ultra96.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(I2CTCA9554RoRegister, 0, 0xaa)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CTCA9554RoRegister, 0xaa)

TEST(I2CTCA9554RoRegister, EncodeRead)
{
	using namespace hxcomm::vx;

	typename I2CTCA9554RoRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CTCA9554RoRegisterOnTCA9554::inputs,
	    halco::hicann_dls::vx::TCA9554OnBoard());
	auto messages = I2CTCA9554RoRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 3);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_tca9554_base_address, false));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_tca9554_base_address + i2c_over_omnibus_stop), true));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x0)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_read_stop);
}

TEST(I2CTCA9554RoRegister, Ostream)
{
	I2CTCA9554RoRegister obj;
	obj.set(I2CTCA9554RoRegister::Value(170));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CTCA9554RoRegister(0d170 0xaa 0b10101010)");
}
