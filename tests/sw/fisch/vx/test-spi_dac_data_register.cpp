#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/spi.h"

TEST(SPIDACDataRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SPIDACDataRegister());

	SPIDACDataRegister default_config;
	EXPECT_EQ(default_config.get(), SPIDACDataRegister::Value());

	SPIDACDataRegister::Value value(0x678);
	SPIDACDataRegister value_config(value);
	EXPECT_EQ(value_config.get(), value);

	SPIDACDataRegister::Value other_value(0x321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	SPIDACDataRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(SPIDACDataRegister, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;
	using namespace halco::hicann_dls::vx;

	SPIDACDataRegister obj;
	obj.set(SPIDACDataRegister::Value(12));

	typename SPIDACDataRegister::coordinate_type coord(halco::common::Enum(3));
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 8);
	auto addr = ut_message_to_fpga<instruction::omnibus_to_fpga::address>(
	    instruction::omnibus_to_fpga::address::payload_type(
	        executor_omnibus_mask | spi_over_omnibus_mask | (coord.toDACOnBoard() + 2), false));
	auto message_addr_1 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1,
	    ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	        instruction::omnibus_to_fpga::data::payload_type(
	            (coord.toSPIDACDataRegisterOnDAC().toEnum() << 12) | (obj.get() >> CHAR_BIT))));
	auto message_addr_2 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	                        instruction::omnibus_to_fpga::data::payload_type(
	                            spi_over_omnibus_stop_bit | static_cast<uint8_t>(obj.get()))));
	auto message_addr_3 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, addr);
	auto message_data_3 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3,
	    ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	        instruction::omnibus_to_fpga::data::payload_type(
	            (1 << 7) | (SPIDACControlRegisterOnBoard(
	                            SPIDACControlRegisterOnDAC::LDAC, coord.toDACOnBoard())
	                            .toSPIDACControlRegisterOnDAC()
	                        << 5))));
	auto message_addr_4 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::address>>(messages.at(6));
	EXPECT_EQ(message_addr_4, addr);
	auto message_data_4 =
	    boost::get<ut_message_to_fpga<instruction::omnibus_to_fpga::data>>(messages.at(7));
	EXPECT_EQ(
	    message_data_4,
	    ut_message_to_fpga<instruction::omnibus_to_fpga::data>(
	        instruction::omnibus_to_fpga::data::payload_type(0x2ul | spi_over_omnibus_stop_bit)));
}

TEST(SPIDACDataRegister, Ostream)
{
	using namespace fisch::vx;

	SPIDACDataRegister obj(SPIDACDataRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIDACDataRegister(0d13 0xd 0b000000001101)");
}

TEST(SPIDACDataRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	SPIDACDataRegister obj1, obj2;
	obj1.set(SPIDACDataRegister::Value(0x678));

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
