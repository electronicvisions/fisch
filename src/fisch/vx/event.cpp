#include "fisch/vx/event.h"

#include "fisch/vx/encode.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

template <size_t NumPack>
typename SpikePackToChip<NumPack>::Value const& SpikePackToChip<NumPack>::get() const
{
	return m_value;
}

template <size_t NumPack>
void SpikePackToChip<NumPack>::set(Value const& value)
{
	m_value = value;
}

template <size_t NumPack>
bool SpikePackToChip<NumPack>::operator==(SpikePackToChip<NumPack> const& other) const
{
	return (m_value == other.m_value);
}

template <size_t NumPack>
bool SpikePackToChip<NumPack>::operator!=(SpikePackToChip<NumPack> const& other) const
{
	return !(*this == other);
}

template <size_t NumPack>
void SpikePackToChip<NumPack>::encode_write(
    coordinate_type const& /*coord*/, UTMessageToFPGABackEmplacer& target) const
{
	typename hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>::Payload::spikes_type
	    spikes;
	std::transform(m_value.cbegin(), m_value.cend(), spikes.begin(), [](auto const& label) {
		return typename decltype(spikes)::value_type(label.value());
	});

	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>>(
	    typename hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>::Payload(spikes)));
}

template class SpikePackToChip<1>;
template class SpikePackToChip<2>;
template class SpikePackToChip<3>;


std::ostream& operator<<(std::ostream& os, SpikeFromChip const& event)
{
	std::stringstream ss;
	os << "SpikeFromChip(" << event.label << ", " << event.chip_time << ", " << event.fpga_time
	   << ")";
	return os;
}

bool SpikeFromChip::operator==(SpikeFromChip const& other) const
{
	return (label == other.label) && (chip_time == other.chip_time) &&
	       (fpga_time == other.fpga_time);
}

bool SpikeFromChip::operator!=(SpikeFromChip const& other) const
{
	return !(*this == other);
}


std::ostream& operator<<(std::ostream& os, MADCSampleFromChip const& event)
{
	std::stringstream ss;
	os << "MADCSampleFromChip(" << event.value << ", " << event.channel << ", " << event.chip_time
	   << ", " << event.fpga_time << ")";
	return os;
}

bool MADCSampleFromChip::operator==(MADCSampleFromChip const& other) const
{
	return (value == other.value) && (channel == other.channel) && (chip_time == other.chip_time) &&
	       (fpga_time == other.fpga_time);
}

bool MADCSampleFromChip::operator!=(MADCSampleFromChip const& other) const
{
	return !(*this == other);
}


HighspeedLinkNotification::Value HighspeedLinkNotification::get_value() const
{
	return m_value;
}

void HighspeedLinkNotification::set_value(Value const& value)
{
	m_value = value;
}

FPGATime HighspeedLinkNotification::get_fpga_time() const
{
	return m_fpga_time;
}

void HighspeedLinkNotification::set_fpga_time(FPGATime const& value)
{
	m_fpga_time = value;
}

std::ostream& operator<<(std::ostream& os, HighspeedLinkNotification const& event)
{
	std::stringstream ss;
	os << "HighspeedLinkNotification(" << event.m_value << ", " << event.m_fpga_time << ")";
	return os;
}

bool HighspeedLinkNotification::operator==(HighspeedLinkNotification const& other) const
{
	return (m_value == other.m_value) && (m_fpga_time == other.m_fpga_time);
}

bool HighspeedLinkNotification::operator!=(HighspeedLinkNotification const& other) const
{
	return !(*this == other);
}


TimeoutNotification::Value TimeoutNotification::get_value() const
{
	return m_value;
}

void TimeoutNotification::set_value(Value const& value)
{
	m_value = value;
}

TimeoutNotification::TraceStallCnt TimeoutNotification::get_trace_stall() const
{
	return m_trace_stall;
}

void TimeoutNotification::set_trace_stall(TraceStallCnt const& value)
{
	m_trace_stall = value;
}

TimeoutNotification::OmnibusReadInFlightCnt TimeoutNotification::get_omnibus_reads() const
{
	return m_omnibus_reads;
}

void TimeoutNotification::set_omnibus_reads(OmnibusReadInFlightCnt const& value)
{
	m_omnibus_reads = value;
}

FPGATime TimeoutNotification::get_fpga_time() const
{
	return m_fpga_time;
}

void TimeoutNotification::set_fpga_time(FPGATime const& value)
{
	m_fpga_time = value;
}

std::ostream& operator<<(std::ostream& os, TimeoutNotification const& event)
{
	std::stringstream ss;
	os << "TimeoutNotification(" << event.m_value << ", " << event.m_trace_stall << ", "
	   << event.m_omnibus_reads << ", " << event.m_fpga_time << ")";
	return os;
}

bool TimeoutNotification::operator==(TimeoutNotification const& other) const
{
	return (m_value == other.m_value) && (m_trace_stall == other.m_trace_stall) &&
	       (m_omnibus_reads == other.m_omnibus_reads) && (m_fpga_time == other.m_fpga_time);
}

bool TimeoutNotification::operator!=(TimeoutNotification const& other) const
{
	return !(*this == other);
}

} // namespace fisch::vx
