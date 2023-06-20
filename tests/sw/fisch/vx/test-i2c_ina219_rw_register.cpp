#include <gtest/gtest.h>

#include "fisch/cerealization.tcc"
#include "fisch/vx/encode.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(I2CINA219RwRegister, 0, 0x1234)
FISCH_TEST_NUMBER_REGISTER_CEREAL(I2CINA219RwRegister, 0x1234)

TEST(I2CINA219RwRegister, EncodeRead)
{
	using namespace hxcomm::vx;

	typename I2CINA219RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CINA219RwRegisterOnINA219::calibration,
	    halco::hicann_dls::vx::INA219OnBoard::vdd12_analog);
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	I2CINA219RwRegister::encode_read(coord, emplacer);

	EXPECT_EQ(messages.size(), 4);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2), false));

	auto addr_read = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2), true));

	auto addr_read_stop = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ina219_base_address + 2) | i2c_over_omnibus_stop, true));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x5)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr_read);
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(message_addr_3, addr_read_stop);
}

TEST(I2CINA219RwRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	typename I2CINA219RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CINA219RwRegisterOnINA219::calibration,
	    halco::hicann_dls::vx::INA219OnBoard::vdd12_analog);
	I2CINA219RwRegister reg(I2CINA219RwRegister::Value(0x1234));
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	reg.encode_write(coord, emplacer);

	EXPECT_EQ(messages.size(), 6);
	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2)));

	auto stop_addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ina219_base_address + 2) | i2c_over_omnibus_stop));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x5)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x12)));
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, stop_addr);
	auto message_data_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x34)));
}

TEST(I2CINA219RwRegister, Ostream)
{
	I2CINA219RwRegister obj;
	obj.set(I2CINA219RwRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CINA219RwRegister(0d13 0xd 0b0000000000001101)");
}
