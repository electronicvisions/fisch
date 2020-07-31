#include <gtest/gtest.h>

#include "fisch/vx/spi.h"

#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/spi.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(SPIDACControlRegister, 0, 0x2000, 12)

TEST(SPIDACControlRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	SPIDACControlRegister obj;
	obj.set(SPIDACControlRegister::Value(12));

	typename SPIDACControlRegister::coordinate_type coord(halco::common::Enum(3));
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 4);
	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                        instruction::omnibus_to_fpga::Address::Payload(
	                            spi_over_omnibus_mask | (coord.toDACOnBoard() + 2), false)));
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(
	                            (1 << 7) | (coord.toSPIDACControlRegisterOnDAC() << 5))));
	auto message_addr_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(
	    message_addr_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                        instruction::omnibus_to_fpga::Address::Payload(
	                            spi_over_omnibus_mask | (coord.toDACOnBoard() + 2), false)));
	auto message_data_2 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_2, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                        instruction::omnibus_to_fpga::Data::Payload(
	                            obj.get().value() | spi_over_omnibus_stop_bit)));
}

TEST(SPIDACControlRegister, Ostream)
{
	SPIDACControlRegister obj(SPIDACControlRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIDACControlRegister(0d13 0xd 0b0000000001101)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(SPIDACControlRegister, 0x678)
