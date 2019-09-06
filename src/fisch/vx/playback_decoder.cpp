#include "fisch/vx/playback_decoder.h"

#include "hate/math.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

PlaybackDecoder::PlaybackDecoder(
    jtag_queue_type& jtag_queue,
    omnibus_queue_type& omnibus_queue,
    spike_queue_type& spike_queue,
    madc_sample_queue_type& madc_sample_queue,
    spike_pack_counts_type& spike_pack_counts,
    madc_sample_pack_counts_type& madc_sample_pack_counts) :
    m_jtag_queue(jtag_queue),
    m_omnibus_queue(omnibus_queue),
    m_spike_queue(spike_queue),
    m_madc_sample_queue(madc_sample_queue),
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
	m_jtag_queue.clear();
	m_omnibus_queue.clear();
	m_spike_queue.clear();
	m_madc_sample_queue.clear();
	m_spike_pack_counts.fill(0);
	m_madc_sample_pack_counts.fill(0);
	m_time_current = FPGATime(0);
}

void PlaybackDecoder::process(ut_message_from_fpga_jtag_type const& message)
{
	m_jtag_queue.push_back(message, m_time_current);
}

void PlaybackDecoder::process(ut_message_from_fpga_omnibus_type const& message)
{
	m_omnibus_queue.push_back(message, m_time_current);
}

void PlaybackDecoder::process(ut_message_from_fpga_halt_type const&)
{ /* do nothing */
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
		SpikeLabel label(spike.get_neuron_label(), spike.get_spl1_address());
		m_spike_queue.push_back(SpikeFromChipEvent(
		    SpikeFromChip(label, calculate_chip_time(spike.get_timestamp())), m_time_current));
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
		        MADCSampleFromChip::Value(madc_sample.get_value()),
		        calculate_chip_time(madc_sample.get_timestamp())),
		    m_time_current));
	}
	m_madc_sample_pack_counts[halco::hicann_dls::vx::MADCSamplePackFromFPGAOnDLS(N)]++;
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
