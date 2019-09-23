#include <gtest/gtest.h>

#include "fisch/vx/playback_decoder.h"

#include "hxcomm/vx/utmessage.h"

TEST(PlaybackDecoder, JTAG)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	typedef instruction::jtag_from_hicann::Data jtag_instruction_type;
	typedef instruction::omnibus_from_fpga::Data omnibus_instruction_type;

	PlaybackDecoder::jtag_queue_type jtag_queue;
	PlaybackDecoder::omnibus_queue_type omnibus_queue;
	PlaybackDecoder::spike_queue_type spike_queue;
	PlaybackDecoder::madc_sample_queue_type madc_sample_queue;
	PlaybackDecoder::spike_pack_counts_type spike_pack_counts;
	PlaybackDecoder::madc_sample_pack_counts_type madc_sample_pack_counts;
	EXPECT_EQ(jtag_queue.size(), 0);
	EXPECT_EQ(omnibus_queue.size(), 0);
	EXPECT_EQ(spike_queue.size(), 0);
	EXPECT_EQ(madc_sample_queue.size(), 0);

	PlaybackDecoder decoder(
	    jtag_queue, omnibus_queue, spike_queue, madc_sample_queue, spike_pack_counts,
	    madc_sample_pack_counts);

	decoder(UTMessageFromFPGA<instruction::from_fpga_system::Halt>());
	EXPECT_EQ(jtag_queue.size(), 0);
	EXPECT_EQ(omnibus_queue.size(), 0);

	auto jtag_message =
	    UTMessageFromFPGA<jtag_instruction_type>(jtag_instruction_type::Payload(0x1234));
	decoder(jtag_message);
	EXPECT_EQ(jtag_queue.size(), 1);
	EXPECT_EQ(jtag_queue.at(0).message, jtag_message);
	EXPECT_EQ(omnibus_queue.size(), 0);
	jtag_queue.clear();

	auto omnibus_message =
	    UTMessageFromFPGA<omnibus_instruction_type>(omnibus_instruction_type::Payload(0x1234));
	decoder(omnibus_message);
	EXPECT_EQ(omnibus_queue.size(), 1);
	EXPECT_EQ(omnibus_queue.at(0).message, omnibus_message);
	EXPECT_EQ(jtag_queue.size(), 0);
}
