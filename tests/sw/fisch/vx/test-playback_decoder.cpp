#include <gtest/gtest.h>

#include "fisch/vx/playback_decoder.h"

TEST(PlaybackDecoder, JTAG)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	typedef instruction::jtag_from_hicann::Data jtag_instruction_type;
	typedef instruction::omnibus_from_fpga::Data omnibus_instruction_type;

	PlaybackDecoder::jtag_queue_type jtag_queue;
	PlaybackDecoder::omnibus_queue_type omnibus_queue;
	EXPECT_EQ(jtag_queue.size(), 0);
	EXPECT_EQ(omnibus_queue.size(), 0);

	PlaybackDecoder decoder(jtag_queue, omnibus_queue);

	decoder(UTMessageFromFPGA<instruction::from_fpga_system::Halt>());
	EXPECT_EQ(jtag_queue.size(), 0);
	EXPECT_EQ(omnibus_queue.size(), 0);

	auto jtag_message =
	    UTMessageFromFPGA<jtag_instruction_type>(jtag_instruction_type::Payload(0x1234));
	decoder(jtag_message);
	EXPECT_EQ(jtag_queue.size(), 1);
	EXPECT_EQ(jtag_queue.at(0), jtag_message);
	EXPECT_EQ(omnibus_queue.size(), 0);
	jtag_queue.clear();

	auto omnibus_message =
	    UTMessageFromFPGA<omnibus_instruction_type>(omnibus_instruction_type::Payload(0x1234));
	decoder(omnibus_message);
	EXPECT_EQ(omnibus_queue.size(), 1);
	EXPECT_EQ(omnibus_queue.at(0), omnibus_message);
	EXPECT_EQ(jtag_queue.size(), 0);
}
