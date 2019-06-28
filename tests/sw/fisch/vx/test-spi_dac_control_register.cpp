#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/spi.h"

TEST(SPIDACControlRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SPIDACControlRegister());

	SPIDACControlRegister default_config;
	EXPECT_EQ(default_config.get(), SPIDACControlRegister::Value());

	SPIDACControlRegister::Value value(0x678);
	SPIDACControlRegister value_config(value);
	EXPECT_EQ(value_config.get(), value);

	SPIDACControlRegister::Value other_value(0x321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	SPIDACControlRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(SPIDACControlRegister, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	SPIDACControlRegister obj;
	obj.set(SPIDACControlRegister::Value(12));

	typename SPIDACControlRegister::coordinate_type coord(halco::common::Enum(3));
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 4);
	auto message_addr_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr_1,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	        instruction::omnibus_to_fpga::Address::Payload(
	            executor_omnibus_mask | spi_over_omnibus_mask | (coord.toDACOnBoard() + 2),
	            false)));
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(
	                            (1 << 7) | (coord.toSPIDACControlRegisterOnDAC() << 5))));
	auto message_addr_2 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(
	    message_addr_2,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	        instruction::omnibus_to_fpga::Address::Payload(
	            executor_omnibus_mask | spi_over_omnibus_mask | (coord.toDACOnBoard() + 2),
	            false)));
	auto message_data_2 =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(
	                            obj.get().value() | spi_over_omnibus_stop_bit)));
}

TEST(SPIDACControlRegister, Ostream)
{
	using namespace fisch::vx;

	SPIDACControlRegister obj(SPIDACControlRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIDACControlRegister(0d13 0xd 0b0000000001101)");
}

TEST(SPIDACControlRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	SPIDACControlRegister obj1, obj2;
	obj1.set(SPIDACControlRegister::Value(0x678));

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
