#pragma once

#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

/**
 * Playback decoder decoding a linear stream of UT messages into data response queues.
 */
class PlaybackDecoder
{
public:
	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::jtag_from_hicann::Data>
	    ut_message_from_fpga_jtag_type;
	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    ut_message_from_fpga_omnibus_type;

	typedef std::vector<ut_message_from_fpga_jtag_type> jtag_queue_type;
	typedef std::vector<ut_message_from_fpga_omnibus_type> omnibus_queue_type;

	typedef hxcomm::vx::UTMessageFromFPGAVariant ut_message_from_fpga_variant_type;

	/**
	 * Construct decoder from JTAG and Omnibus queue references to put messages in.
	 * @param jtag_queue Reference to a JTAG message queue
	 * @param omnibus_queue Reference to a Omnibus message queue
	 */
	PlaybackDecoder(jtag_queue_type& jtag_queue, omnibus_queue_type& omnibus_queue);

	/**
	 * Process a message.
	 * @param message UT message variant to process
	 */
	void operator()(ut_message_from_fpga_variant_type const& message);

private:
	void process(ut_message_from_fpga_jtag_type const& message);
	void process(ut_message_from_fpga_omnibus_type const& message);

	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::from_fpga_system::Halt>
	    ut_message_from_fpga_halt_type;
	void process(ut_message_from_fpga_halt_type const& message);

	jtag_queue_type& m_jtag_queue;
	omnibus_queue_type& m_omnibus_queue;
};

} // namespace fisch::vx
