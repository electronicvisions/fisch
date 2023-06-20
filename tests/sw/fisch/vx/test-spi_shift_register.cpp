#include <gtest/gtest.h>

#include "fisch/vx/spi.h"

#include "fisch/vx/encode.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/spi.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(SPIShiftRegister, 0, 0x1000000, 12)

TEST(SPIShiftRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	SPIShiftRegister obj;
	obj.set(SPIShiftRegister::Value(0x123456));

	typename SPIShiftRegister::coordinate_type coord;
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(coord, emplacer);

	EXPECT_EQ(messages.size(), 6);
	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(1ul | spi_over_omnibus_mask, false));

	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(message_addr_1, addr);
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x12)));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(message_addr_2, addr);
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(0x34)));
	auto message_addr_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(message_addr_3, addr);
	auto message_data_3 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    message_data_3, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(fpga_omnibus_mask | 0x56)));
}

TEST(SPIShiftRegister, Ostream)
{
	SPIShiftRegister obj(SPIShiftRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIShiftRegister(0d13 0xd 0b000000000000000000001101)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(SPIShiftRegister, 0x345678)
