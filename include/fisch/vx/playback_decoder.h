#pragma once
#include <tuple>

#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/chip_time.h"
#include "fisch/vx/decode_message_types.h"
#include "fisch/vx/event.h"
#include "fisch/vx/fpga_time.h"
#include "fisch/vx/genpybind.h"
#include "halco/common/typed_array.h"
#include "halco/hicann-dls/vx/event.h"
#include "hate/type_list.h"

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
	struct TimedResponseQueue
	{
	private:
		struct SizeConstraintChecker
		{
			SizeConstraintChecker(TimedResponseQueue const& queue) : queue(queue) {}
			~SizeConstraintChecker() { assert(queue.m_messages.size() == queue.m_times.size()); }

			TimedResponseQueue const& queue;
		};

		std::vector<MessageT> m_messages;
		std::vector<FPGATime> m_times;

	public:
		typedef MessageT message_type;

		std::vector<MessageT> const& get_messages() const { return m_messages; }
		std::vector<FPGATime> const& get_times() const { return m_times; }

		void push_back(MessageT const& message, FPGATime const& time)
		{
			auto checker = SizeConstraintChecker(*this);
			static_cast<void>(checker);

			m_messages.push_back(message);
			m_times.push_back(time);
		}

		size_t size() const { return m_messages.size(); }

		void clear()
		{
			m_messages.clear();
			m_times.clear();
		}

		TimedResponseQueue() : m_messages(), m_times() {}
		TimedResponseQueue(
		    std::vector<message_type> const& messages, std::vector<FPGATime> const& times) :
		    m_messages(messages),
		    m_times(times)
		{
			auto checker = SizeConstraintChecker(*this);
			static_cast<void>(checker);
		}

		bool operator==(TimedResponseQueue const& other) const
		{
			return m_messages == other.m_messages && m_times == other.m_times;
		}

		bool operator!=(TimedResponseQueue const& other) const { return !(*this == other); }
	};

	template <typename TL>
	struct ToResponseQueueTuple;

	template <typename... Ts>
	struct ToResponseQueueTuple<hate::type_list<Ts...>>
	{
		typedef std::tuple<TimedResponseQueue<Ts>...> type;
	};

	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::jtag_from_hicann::Data>
	    ut_message_from_fpga_jtag_type;
	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    ut_message_from_fpga_omnibus_type;
	typedef hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::from_fpga_system::Loopback>
	    ut_message_from_fpga_loopback_type;

	typedef typename ToResponseQueueTuple<detail::decode_message_types>::type response_queue_type;
	typedef std::vector<SpikeFromChipEvent> spike_queue_type;
	typedef std::vector<MADCSampleFromChipEvent> madc_sample_queue_type;
	typedef std::vector<HighspeedLinkNotification> highspeed_link_notification_queue_type;
	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::SpikePackFromFPGAOnDLS>
	    spike_pack_counts_type;
	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::MADCSamplePackFromFPGAOnDLS>
	    madc_sample_pack_counts_type;
	typedef std::vector<TimeoutNotification> timeout_notification_queue_type;

	typedef hxcomm::vx::UTMessageFromFPGAVariant ut_message_from_fpga_variant_type;

	/**
	 * Construct decoder from JTAG and Omnibus queue references to put messages in.
	 * @param response_queue Reference to response message queue
	 * @param spike_queue Reference to a spike event message queue
	 * @param madc_sample_queue Reference to a MADC sample event message queue
	 * @param highspeed_link_notification_queue Reference to a highspeed link notification queue
	 * @param spike_pack_counts Reference to a spike pack count array
	 * @param madc_sample_pack_counts Reference to a MADC sample pack count array
	 */
	PlaybackDecoder(
	    response_queue_type& response_queue,
	    spike_queue_type& spike_queue,
	    madc_sample_queue_type& madc_sample_queue,
	    highspeed_link_notification_queue_type& highspeed_link_notification_queue,
	    spike_pack_counts_type& spike_pack_counts,
	    madc_sample_pack_counts_type& madc_sample_pack_counts,
	    timeout_notification_queue_type& timeout_notification_queue);

	/**
	 * Process a message.
	 * @param message UT message variant to process
	 */
	void operator()(ut_message_from_fpga_variant_type const& message);

	/**
	 * Clear state leading to FPGA time annotation reset and clearing of all queues and event pack
	 * counters.
	 */
	void clear();

private:
	void process(ut_message_from_fpga_jtag_type const& message);
	void process(ut_message_from_fpga_omnibus_type const& message);

	void process(ut_message_from_fpga_loopback_type const& message);

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

	typedef hxcomm::vx::UTMessageFromFPGA<
	    hxcomm::vx::instruction::from_fpga_system::HighspeedLinkNotification>
	    ut_message_from_fpga_highspeed_link_notification_type;
	void process(ut_message_from_fpga_highspeed_link_notification_type const& message);
	typedef hxcomm::vx::UTMessageFromFPGA<
	    hxcomm::vx::instruction::from_fpga_system::TimeoutNotification>
	    ut_message_from_fpga_timeout_notification_type;
	void process(ut_message_from_fpga_timeout_notification_type const& message);

	ChipTime calculate_chip_time(uint8_t timestamp) const;

	response_queue_type& m_response_queue;
	spike_queue_type& m_spike_queue;
	madc_sample_queue_type& m_madc_sample_queue;
	highspeed_link_notification_queue_type& m_highspeed_link_notification_queue;
	spike_pack_counts_type& m_spike_pack_counts;
	madc_sample_pack_counts_type& m_madc_sample_pack_counts;
	timeout_notification_queue_type& m_timeout_notification_queue;

	FPGATime m_time_current;
};

} // namespace fisch::vx
