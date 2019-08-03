#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_NUMBER_REGISTER_GENERAL(JTAGIdCode, 0, 12)

TEST(JTAGIdCode, EncodeRead)
{
	using namespace hxcomm::vx;

	JTAGIdCode obj;

	auto messages = obj.encode_read(typename JTAGIdCode::coordinate_type());

	EXPECT_EQ(messages.size(), 2);
	auto message_ins = boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins.decode(), instruction::to_fpga_jtag::Ins::IDCODE);
	auto message_data =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
	EXPECT_EQ(message_data.decode().get_payload(), 0);
	EXPECT_EQ(message_data.decode().get_keep_response(), true);
	EXPECT_EQ(message_data.decode().get_num_bits(), sizeof(uint32_t) * CHAR_BIT);
}

TEST(JTAGIdCode, Ostream)
{
	JTAGIdCode obj;

	JTAGIdCode::Value id(0x12345678);
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::jtag_from_hicann::Data> message(
	    hxcomm::vx::instruction::jtag_from_hicann::Data::Payload(id.value()));

	obj.decode({message});

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGIdCode(0x12345678)");
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(JTAGIdCode, 0x12345678)
