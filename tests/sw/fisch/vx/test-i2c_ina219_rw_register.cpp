#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"

TEST(I2CINA219RwRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(I2CINA219RwRegister());

	I2CINA219RwRegister default_config;
	EXPECT_EQ(default_config.get(), I2CINA219RwRegister::Value());

	I2CINA219RwRegister::Value value(0x1234);
	I2CINA219RwRegister value_config;
	value_config.set(value);
	EXPECT_EQ(value_config.get(), value);

	I2CINA219RwRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(I2CINA219RwRegister, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	typename I2CINA219RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CINA219RwRegisterOnINA219::calibration,
	    halco::hicann_dls::vx::INA219OnBoard::vdd12_analog);
	auto messages = I2CINA219RwRegister::encode_read(coord);

	EXPECT_EQ(messages.size(), 4);
	auto addr_write = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2), false));

	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2), true));

	auto stop_addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ina219_base_address + 2) | i2c_over_omnibus_stop, true));

	auto message_addr_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr_write);
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x5)));
	auto message_addr_2 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_addr_3 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(message_addr_3, stop_addr);
}

TEST(I2CINA219RwRegister, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	typename I2CINA219RwRegister::coordinate_type coord(
	    halco::hicann_dls::vx::I2CINA219RwRegisterOnINA219::calibration,
	    halco::hicann_dls::vx::INA219OnBoard::vdd12_analog);
	I2CINA219RwRegister reg(I2CINA219RwRegister::Value(0x1234));
	auto messages = reg.encode_write(coord);

	EXPECT_EQ(messages.size(), 6);
	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload((i2c_ina219_base_address + 2)));

	auto stop_addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        (i2c_ina219_base_address + 2) | i2c_over_omnibus_stop));

	auto message_addr_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x5)));
	auto message_addr_2 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x12)));
	auto message_addr_3 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, stop_addr);
	auto message_data_3 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x34)));
}

TEST(I2CINA219RwRegister, Ostream)
{
	using namespace fisch::vx;

	I2CINA219RwRegister obj;
	obj.set(I2CINA219RwRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "I2CINA219RwRegister(0d13 0xd 0b0000000000001101)");
}

TEST(I2CINA219RwRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	I2CINA219RwRegister obj1, obj2;
	obj1.set(I2CINA219RwRegister::Value(0x1234));

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
