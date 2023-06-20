#include <gtest/gtest.h>

#include "fisch/cerealization.tcc"
#include "fisch/vx/encode.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(I2CTCA9554RwRegister, 0, 0x55)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CTCA9554RwRegister, 0x55)

TEST(I2CTCA9554RwRegister, EncodeRead)
{
	using namespace hxcomm::vx;

	typename I2CTCA9554RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CTCA9554RwRegisterOnTCA9554::polarity,
	    halco::hicann_dls::vx::TCA9554OnBoard());
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	I2CTCA9554RwRegister::encode_read(coord, emplacer);

	EXPECT_EQ(messages.size(), 3);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_tca9554_base_address, false));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_tca9554_base_address | i2c_over_omnibus_stop, true));

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
	EXPECT_EQ(message_addr_2, addr_read_stop);
}

TEST(I2CTCA9554RwRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	typename I2CTCA9554RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CTCA9554RwRegisterOnTCA9554::polarity,
	    halco::hicann_dls::vx::TCA9554OnBoard());
	I2CTCA9554RwRegister reg(I2CTCA9554RwRegister::Value(0x55));
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	reg.encode_write(coord, emplacer);

	EXPECT_EQ(messages.size(), 4);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_tca9554_base_address));

	auto addr_write_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_tca9554_base_address | i2c_over_omnibus_stop));

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
	EXPECT_EQ(message_addr_2, addr_write_stop);
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x55)));
}

TEST(I2CTCA9554RwRegister, Ostream)
{
	I2CTCA9554RwRegister obj;
	obj.set(I2CTCA9554RwRegister::Value(85));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CTCA9554RwRegister(0d85 0x55 0b01010101)");
}
