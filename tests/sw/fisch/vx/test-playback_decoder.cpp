#include <gtest/gtest.h>

#include "fisch/vx/playback_decoder.h"

#include "hxcomm/vx/utmessage.h"

#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>

TEST(PlaybackDecoder, JTAG)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	typedef instruction::jtag_from_hicann::Data jtag_instruction_type;
	typedef instruction::omnibus_from_fpga::Data omnibus_instruction_type;

	PlaybackDecoder::response_queue_type response_queue;
	PlaybackDecoder::spike_queue_type spike_queue;
	PlaybackDecoder::madc_sample_queue_type madc_sample_queue;
	PlaybackDecoder::spike_pack_counts_type spike_pack_counts;
	PlaybackDecoder::madc_sample_pack_counts_type madc_sample_pack_counts;
	boost::hana::for_each(response_queue, [](auto const& q) { EXPECT_EQ(q.size(), 0); });
	EXPECT_EQ(spike_queue.size(), 0);
	EXPECT_EQ(madc_sample_queue.size(), 0);

	PlaybackDecoder decoder(
	    response_queue, spike_queue, madc_sample_queue, spike_pack_counts, madc_sample_pack_counts);

	decoder(UTMessageFromFPGA<instruction::from_fpga_system::Loopback>());
	boost::hana::for_each(response_queue, [](auto const& q) { EXPECT_EQ(q.size(), 0); });

	auto jtag_message =
	    UTMessageFromFPGA<jtag_instruction_type>(jtag_instruction_type::Payload(0x1234));
	auto omnibus_message =
	    UTMessageFromFPGA<omnibus_instruction_type>(omnibus_instruction_type::Payload(0x1234));
	auto& jtag_queue = std::get<
	    hate::index_type_list_by_type<decltype(jtag_message), detail::decode_message_types>::value>(
	    response_queue);
	auto const& omnibus_queue = std::get<hate::index_type_list_by_type<
	    decltype(omnibus_message), detail::decode_message_types>::value>(response_queue);

	decoder(jtag_message);
	EXPECT_EQ(jtag_queue.size(), 1);
	EXPECT_EQ(jtag_queue.get_messages().at(0), jtag_message);
	EXPECT_EQ(omnibus_queue.size(), 0);
	jtag_queue.clear();

	decoder(omnibus_message);
	EXPECT_EQ(omnibus_queue.size(), 1);
	EXPECT_EQ(omnibus_queue.get_messages().at(0), omnibus_message);
	EXPECT_EQ(jtag_queue.size(), 0);
}
