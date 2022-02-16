#include "fisch/vx/event.h"

#include "cereal/types/array.hpp"
#include "fisch/cerealization.h"
#include "halco/common/cerealization_geometry.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

template <size_t NumPack>
SpikePackToChip<NumPack>::SpikePackToChip() : m_value()
{}

template <size_t NumPack>
SpikePackToChip<NumPack>::SpikePackToChip(Value const& labels) : m_value(labels)
{}

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
std::array<
    hxcomm::vx::UTMessageToFPGAVariant,
    SpikePackToChip<NumPack>::encode_write_ut_message_count>
SpikePackToChip<NumPack>::encode_write(coordinate_type const& /*coord*/) const
{
	typename hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>::Payload::spikes_type
	    spikes;
	std::transform(m_value.cbegin(), m_value.cend(), spikes.begin(), [](auto const& label) {
		return typename decltype(spikes)::value_type(label.value());
	});

	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>>(
	    typename hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>::Payload(spikes))};
}

template <size_t NumPack>
template <typename Archive>
void SpikePackToChip<NumPack>::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

template class SpikePackToChip<1>;
template class SpikePackToChip<2>;
template class SpikePackToChip<3>;

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<1>)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<2>)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<3>)


SpikeLabel SpikeFromChipEvent::get_label() const
{
	return m_label;
}

void SpikeFromChipEvent::set_label(SpikeLabel const& value)
{
	m_label = value;
}

ChipTime SpikeFromChipEvent::get_chip_time() const
{
	return m_chip_time;
}

void SpikeFromChipEvent::set_chip_time(ChipTime const& value)
{
	m_chip_time = value;
}

FPGATime SpikeFromChipEvent::get_fpga_time() const
{
	return m_fpga_time;
}

void SpikeFromChipEvent::set_fpga_time(FPGATime const& value)
{
	m_fpga_time = value;
}

std::ostream& operator<<(std::ostream& os, SpikeFromChipEvent const& event)
{
	std::stringstream ss;
	os << "SpikeFromChipEvent(" << event.m_label << ", " << event.m_chip_time << ", "
	   << event.m_fpga_time << ")";
	return os;
}

bool SpikeFromChipEvent::operator==(SpikeFromChipEvent const& other) const
{
	return (m_label == other.m_label) && (m_chip_time == other.m_chip_time) &&
	       (m_fpga_time == other.m_fpga_time);
}

bool SpikeFromChipEvent::operator!=(SpikeFromChipEvent const& other) const
{
	return !(*this == other);
}

template <class Archive>
void SpikeFromChipEvent::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_label));
	ar(CEREAL_NVP(m_chip_time));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikeFromChipEvent)


MADCSampleFromChipEvent::Value MADCSampleFromChipEvent::get_value() const
{
	return m_value;
}

void MADCSampleFromChipEvent::set_value(Value const& value)
{
	m_value = value;
}

ChipTime MADCSampleFromChipEvent::get_chip_time() const
{
	return m_chip_time;
}

void MADCSampleFromChipEvent::set_chip_time(ChipTime const& value)
{
	m_chip_time = value;
}

FPGATime MADCSampleFromChipEvent::get_fpga_time() const
{
	return m_fpga_time;
}

void MADCSampleFromChipEvent::set_fpga_time(FPGATime const& value)
{
	m_fpga_time = value;
}

std::ostream& operator<<(std::ostream& os, MADCSampleFromChipEvent const& event)
{
	std::stringstream ss;
	os << "MADCSampleFromChipEvent(" << event.m_value << ", " << event.m_chip_time << ", "
	   << event.m_fpga_time << ")";
	return os;
}

bool MADCSampleFromChipEvent::operator==(MADCSampleFromChipEvent const& other) const
{
	return (m_value == other.m_value) && (m_chip_time == other.m_chip_time) &&
	       (m_fpga_time == other.m_fpga_time);
}

bool MADCSampleFromChipEvent::operator!=(MADCSampleFromChipEvent const& other) const
{
	return !(*this == other);
}

template <class Archive>
void MADCSampleFromChipEvent::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
	ar(CEREAL_NVP(m_chip_time));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(MADCSampleFromChipEvent)


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

template <class Archive>
void HighspeedLinkNotification::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(HighspeedLinkNotification)


TimeoutNotification::Value TimeoutNotification::get_value() const
{
	return m_value;
}

void TimeoutNotification::set_value(Value const& value)
{
	m_value = value;
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
	os << "TimeoutNotification(" << event.m_value << ", " << event.m_fpga_time << ")";
	return os;
}

bool TimeoutNotification::operator==(TimeoutNotification const& other) const
{
	return (m_value == other.m_value) && (m_fpga_time == other.m_fpga_time);
}

bool TimeoutNotification::operator!=(TimeoutNotification const& other) const
{
	return !(*this == other);
}

template <class Archive>
void TimeoutNotification::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(TimeoutNotification)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::SpikePackToChip<1>, 0)
CEREAL_CLASS_VERSION(fisch::vx::SpikePackToChip<2>, 0)
CEREAL_CLASS_VERSION(fisch::vx::SpikePackToChip<3>, 0)
CEREAL_CLASS_VERSION(fisch::vx::SpikeFromChipEvent, 1)
CEREAL_CLASS_VERSION(fisch::vx::MADCSampleFromChipEvent, 1)
CEREAL_CLASS_VERSION(fisch::vx::HighspeedLinkNotification, 0)
CEREAL_CLASS_VERSION(fisch::vx::TimeoutNotification, 0)
