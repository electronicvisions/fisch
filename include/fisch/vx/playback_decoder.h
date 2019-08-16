#pragma once

#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/event.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/systime.h"
#include "halco/hicann-dls/vx/coordinates.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Playback decoder decoding a linear stream of UT messages into data response queues.
 */
class PlaybackDecoder
{
public:
	/**
	 * Pair of UT message and FPGATime time annotation.
	 * @tparam MesageT UT message type
	 */
	template <typename MessageT>
	struct TimedResponse
	{
		typedef MessageT message_type;

		MessageT message;
		FPGATime time;

		TimedResponse(MessageT message, FPGATime time) : message(message), time(time) {}
	};

	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::jtag_from_hicann::Data>
	    ut_message_from_fpga_jtag_type;
	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    ut_message_from_fpga_omnibus_type;

	typedef std::vector<TimedResponse<ut_message_from_fpga_jtag_type>> jtag_queue_type;
	typedef std::vector<TimedResponse<ut_message_from_fpga_omnibus_type>> omnibus_queue_type;
	typedef std::vector<SpikeFromChipEvent> spike_queue_type;
	typedef std::vector<MADCSampleFromChipEvent> madc_sample_queue_type;
	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::SpikePackFromFPGAOnDLS>
	    spike_pack_counts_type;
	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::MADCSamplePackFromFPGAOnDLS>
	    madc_sample_pack_counts_type;

	typedef hxcomm::vx::UTMessageFromFPGAVariant ut_message_from_fpga_variant_type;

	/**
	 * Construct decoder from JTAG and Omnibus queue references to put messages in.
	 * @param jtag_queue Reference to a JTAG message queue
	 * @param omnibus_queue Reference to a Omnibus message queue
	 * @param spike_queue Reference to a spike event message queue
	 * @param madc_sample_queue Reference to a MADC sample event message queue
	 * @param spike_pack_counts Reference to a spike pack count array
	 * @param madc_sample_pack_counts Reference to a MADC sample pack count array
	 */
	PlaybackDecoder(
	    jtag_queue_type& jtag_queue,
	    omnibus_queue_type& omnibus_queue,
	    spike_queue_type& spike_queue,
	    madc_sample_queue_type& madc_sample_queue,
	    spike_pack_counts_type& spike_pack_counts,
	    madc_sample_pack_counts_type& madc_sample_pack_counts);

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

	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::timing_from_fpga::Systime>
	    ut_message_from_fpga_systime_type;
	void process(ut_message_from_fpga_systime_type const& message);

	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::timing_from_fpga::Sysdelta>
	    ut_message_from_fpga_sysdelta_type;
	void process(ut_message_from_fpga_sysdelta_type const& message);

	template <size_t N>
	void process(
	    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::event_from_fpga::SpikePack<N>> const&
	        message);

	template <size_t N>
	void process(hxcomm::vx::UTMessageFromFPGA<
	             hxcomm::vx::instruction::event_from_fpga::MADCSamplePack<N>> const& message);

	ChipTime calculate_chip_time(uint8_t timestamp) const;

	jtag_queue_type& m_jtag_queue;
	omnibus_queue_type& m_omnibus_queue;
	spike_queue_type& m_spike_queue;
	madc_sample_queue_type& m_madc_sample_queue;
	spike_pack_counts_type& m_spike_pack_counts;
	madc_sample_pack_counts_type& m_madc_sample_pack_counts;

	FPGATime m_time_current;
};

} // namespace fisch::vx
