#include <gtest/gtest.h>

#include "fisch/vx/fir_filter.h"

#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/fir_filter.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(FIRFilterRegister, 0, 0x1000000, 12)

TEST(FIRFilterRegister, EncodeWrite)
{
	using namespace hxcomm::vx;

	FIRFilterRegister obj;
	obj.set(FIRFilterRegister::Value(0x123456));

	typename FIRFilterRegister::coordinate_type coord;
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 26);
	auto addr = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(fir_filter_over_omnibus_mask, false));
	auto addr_end = UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	    instruction::omnibus_to_fpga::Address::Payload(fir_filter_over_omnibus_mask + 1, false));

	for (size_t i = 0; i < 12; ++i) {
		auto message_addr =
		    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(i * 2));
		EXPECT_EQ(message_addr, addr);
		auto message_data =
		    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(i * 2 + 1));
		EXPECT_EQ(
		    message_data, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
		                      instruction::omnibus_to_fpga::Data::Payload(
		                          ((obj.get() >> (i * 2)) & 0x3) | (i == 11 ? 0x100 : 0))));
	}
	auto message_addr_end =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(24));
	EXPECT_EQ(message_addr_end, addr_end);
	auto message_data_end =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(25));
	EXPECT_EQ(
	    message_data_end, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                          instruction::omnibus_to_fpga::Data::Payload(0)));
}

TEST(FIRFilterRegister, Ostream)
{
	FIRFilterRegister obj(FIRFilterRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "FIRFilterRegister(0d13 0xd 0b000000000000000000001101)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(FIRFilterRegister, 0x345678)
