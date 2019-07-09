#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"

TEST(I2CIdRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(I2CIdRegister());

	I2CIdRegister default_config;
	EXPECT_EQ(default_config.get(), I2CIdRegister::Value());

	I2CIdRegister::Value value(0x12345678);
	I2CIdRegister value_config;
	value_config.set(value);
	EXPECT_EQ(value_config.get(), value);

	I2CIdRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(I2CIdRegister, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	typename I2CIdRegister::coordinate_type coord;
	auto messages = I2CIdRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 6);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_eeprom_base_address, false));

	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(i2c_eeprom_base_address, true));

	auto stop_addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_eeprom_base_address | i2c_over_omnibus_stop, true));

	auto repeat_start_addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        i2c_eeprom_base_address | i2c_over_omnibus_repeat_start, true));

	auto message_addr_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0xfc)));
	auto message_addr_2 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, repeat_start_addr);
	auto message_addr_3 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(message_addr_3, addr);
	auto message_addr_4 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_4, addr);
	auto message_addr_5 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(5));
	EXPECT_EQ(message_addr_5, stop_addr);
}

TEST(I2CIdRegister, Ostream)
{
	using namespace fisch::vx;

	I2CIdRegister obj;
	obj.set(I2CIdRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CIdRegister(0d13 0xd 0b00000000000000000000000000001101)");
}

TEST(I2CIdRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	I2CIdRegister obj1, obj2;
	obj1.set(I2CIdRegister::Value(0x12345678));

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
