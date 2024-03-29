#include <gtest/gtest.h>

#include "fisch/vx/spi.h"

#include "fisch/vx/encode.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/spi.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(SPIDACDataRegister, 0, 0x1000, 12)

TEST(SPIDACDataRegister, EncodeWrite)
{
	using namespace hxcomm::vx;
	using namespace halco::hicann_dls::vx;

	SPIDACDataRegister obj;
	obj.set(SPIDACDataRegister::Value(0xfc1));

	typename SPIDACDataRegister::coordinate_type coord(halco::common::Enum(3));
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(coord, emplacer);

	EXPECT_EQ(messages.size(), 8);
	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(
	        spi_over_omnibus_mask | ((2 * coord.toDACOnBoard()) + 2), false));
	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(
	            ((coord.toSPIDACDataRegisterOnDAC().toEnum() << 12) | obj.get()) >> CHAR_BIT)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(
	                            spi_over_omnibus_stop_bit | static_cast<uint8_t>(obj.get()))));
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, addr);
	auto message_data_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(
	            (1 << 7) | (SPIDACControlRegisterOnBoard(
	                            SPIDACControlRegisterOnDAC::ldac, coord.toDACOnBoard())
	                            .toSPIDACControlRegisterOnDAC()
	                        << 5))));
	auto message_addr_4 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(6));
	EXPECT_EQ(message_addr_4, addr);
	auto message_data_4 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(7));
	EXPECT_EQ(
	    message_data_4,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(0x2ul | spi_over_omnibus_stop_bit)));
}

TEST(SPIDACDataRegister, Ostream)
{
	SPIDACDataRegister obj(SPIDACDataRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIDACDataRegister(0d13 0xd 0b000000001101)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(SPIDACDataRegister, 0x678)
