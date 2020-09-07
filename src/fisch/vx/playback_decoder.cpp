#include "fisch/vx/playback_decoder.h"

#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>

#include "fisch/common/logger.h"
#include "hate/math.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

PlaybackDecoder::PlaybackDecoder(
    response_queue_type& response_queue,
    spike_queue_type& spike_queue,
    madc_sample_queue_type& madc_sample_queue,
    highspeed_link_notification_queue_type& highspeed_link_notification_queue,
    spike_pack_counts_type& spike_pack_counts,
    madc_sample_pack_counts_type& madc_sample_pack_counts) :
    m_response_queue(response_queue),
    m_spike_queue(spike_queue),
    m_madc_sample_queue(madc_sample_queue),
    m_highspeed_link_notification_queue(highspeed_link_notification_queue),
    m_spike_pack_counts(spike_pack_counts),
    m_madc_sample_pack_counts(madc_sample_pack_counts),
    m_time_current(0)
{}

void PlaybackDecoder::operator()(ut_message_from_fpga_variant_type const& message)
{
	m_time_current = FPGATime(m_time_current + 1);
	boost::apply_visitor([this](auto m) { process(m); }, message);
}

void PlaybackDecoder::clear()
{
	boost::hana::for_each(m_response_queue, [](auto& q) { q.clear(); });
	m_spike_queue.clear();
	m_madc_sample_queue.clear();
	m_spike_pack_counts.fill(0);
	m_madc_sample_pack_counts.fill(0);
	m_time_current = FPGATime(0);
}

void PlaybackDecoder::process(ut_message_from_fpga_jtag_type const& message)
{
	std::get<TimedResponseQueue<ut_message_from_fpga_jtag_type>>(m_response_queue)
	    .push_back(message, m_time_current);
}

void PlaybackDecoder::process(ut_message_from_fpga_omnibus_type const& message)
{
	std::get<TimedResponseQueue<ut_message_from_fpga_omnibus_type>>(m_response_queue)
	    .push_back(message, m_time_current);
}

void PlaybackDecoder::process(ut_message_from_fpga_loopback_type const& message)
{
	if (message.decode() == ut_message_from_fpga_loopback_type::instruction_type::halt) {
		/* do nothing */
	} else {
		std::get<TimedResponseQueue<ut_message_from_fpga_loopback_type>>(m_response_queue)
		    .push_back(message, m_time_current);
	}
}

void PlaybackDecoder::process(ut_message_from_fpga_systime_type const& message)
{
	m_time_current = FPGATime(static_cast<uint64_t>(message.decode()));
}

void PlaybackDecoder::process(ut_message_from_fpga_sysdelta_type const& message)
{
	m_time_current = FPGATime(m_time_current + static_cast<uint64_t>(message.decode()));
}

template <size_t N>
void PlaybackDecoder::process(
    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::event_from_fpga::SpikePack<N>> const&
        message)
{
	auto spikes = message.decode().get_spikes();
	for (auto spike : spikes) {
		SpikeLabel label(static_cast<uint16_t>(spike.get_spike()));
		m_spike_queue.push_back(SpikeFromChipEvent(
		    SpikeFromChip(label, calculate_chip_time(static_cast<uint8_t>(spike.get_timestamp()))),
		    m_time_current));
	}
	m_spike_pack_counts[halco::hicann_dls::vx::SpikePackFromFPGAOnDLS(N)]++;
}

template <size_t N>
void PlaybackDecoder::process(
    hxcomm::vx::UTMessageFromFPGA<
        hxcomm::vx::instruction::event_from_fpga::MADCSamplePack<N>> const& message)
{
	auto madc_samples = message.decode().get_samples();
	for (auto madc_sample : madc_samples) {
		m_madc_sample_queue.push_back(MADCSampleFromChipEvent(
		    MADCSampleFromChip(
		        MADCSampleFromChip::Value(
		            static_cast<MADCSampleFromChip::Value::value_type>(madc_sample.get_value())),
		        calculate_chip_time(static_cast<uint8_t>(madc_sample.get_timestamp()))),
		    m_time_current));
	}
	m_madc_sample_pack_counts[halco::hicann_dls::vx::MADCSamplePackFromFPGAOnDLS(N)]++;
}

void PlaybackDecoder::process(ut_message_from_fpga_highspeed_link_notification_type const& message)
{
	m_highspeed_link_notification_queue.push_back(HighspeedLinkNotification(
	    HighspeedLinkNotification::Value(
	        static_cast<HighspeedLinkNotification::Value::value_type>(message.decode())),
	    m_time_current));
}

void PlaybackDecoder::process(ut_message_from_fpga_timeout_notification_type const& message)
{
	// ignore for now
	auto logger = log4cxx::Logger::getLogger("fisch.PlaybackDecoder.process()");
	FISCH_LOG_WARN(
	    logger, "Timeout notification at " << m_time_current << " ignored: instruction("
	                                       << static_cast<uint32_t>(message.decode()) << ")");
}

ChipTime PlaybackDecoder::calculate_chip_time(uint8_t const timestamp) const
{
	static_assert(
	    hxcomm::vx::instruction::event_from_fpga::Spike::timestamp_size ==
	        hxcomm::vx::instruction::event_from_fpga::MADCSample::timestamp_size,
	    "timestamp_size of Spike and MADCSample don't match.");

	constexpr size_t timestamp_size =
	    hxcomm::vx::instruction::event_from_fpga::Spike::timestamp_size - 1;
	ChipTime retval =
	    ChipTime(((m_time_current >> timestamp_size) << timestamp_size) | (timestamp / 2));
	if (retval > m_time_current)
		retval = ChipTime((retval - hate::math::pow(2, timestamp_size)) & ChipTime::max);
	return retval;
}

} // namespace fisch::vx
