#include "fisch/vx/event.h"
#include "fisch/cerealization.h"

#include "cereal/types/array.hpp"
#include "halco/common/cerealization_geometry.h"

namespace fisch::vx {

SpikeLabel::SpikeLabel() : m_neuron_label(), m_spl1_address() {}

SpikeLabel::SpikeLabel(neuron_label_type const& neuron, spl1_address_type const& spl1) :
    m_neuron_label(neuron),
    m_spl1_address(spl1)
{}

SpikeLabel::neuron_label_type SpikeLabel::get_neuron_label() const
{
	return m_neuron_label;
}

void SpikeLabel::set_neuron_label(neuron_label_type const& value)
{
	m_neuron_label = value;
}

SpikeLabel::spl1_address_type SpikeLabel::get_spl1_address() const
{
	return m_spl1_address;
}

void SpikeLabel::set_spl1_address(spl1_address_type const& value)
{
	m_spl1_address = value;
}

std::ostream& operator<<(std::ostream& os, SpikeLabel const& spike_label)
{
	std::stringstream ss;
	os << "SpikeLabel(" << spike_label.m_neuron_label << ", " << spike_label.m_spl1_address << ")";
	return os;
}

bool SpikeLabel::operator==(SpikeLabel const& other) const
{
	return (m_neuron_label == other.m_neuron_label) && (m_spl1_address == other.m_spl1_address);
}

bool SpikeLabel::operator!=(SpikeLabel const& other) const
{
	return !(*this == other);
}

template <class Archive>
void SpikeLabel::serialize(Archive& ar)
{
	ar(CEREAL_NVP(m_neuron_label));
	ar(CEREAL_NVP(m_spl1_address));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikeLabel)


template <size_t NumPack>
SpikePackToChip<NumPack>::SpikePackToChip() : m_labels()
{}

template <size_t NumPack>
SpikePackToChip<NumPack>::SpikePackToChip(labels_type const& labels) : m_labels(labels)
{}

template <size_t NumPack>
typename SpikePackToChip<NumPack>::labels_type SpikePackToChip<NumPack>::get_labels() const
{
	return m_labels;
}

template <size_t NumPack>
void SpikePackToChip<NumPack>::set_labels(labels_type const& value)
{
	m_labels = value;
}

template <size_t NumPack>
bool SpikePackToChip<NumPack>::operator==(SpikePackToChip<NumPack> const& other) const
{
	return (m_labels == other.m_labels);
}

template <size_t NumPack>
bool SpikePackToChip<NumPack>::operator!=(SpikePackToChip<NumPack> const& other) const
{
	return !(*this == other);
}

template <size_t NumPack>
std::array<
    hxcomm::vx::UTMessageToFPGAVariant,
    SpikePackToChip<NumPack>::encode_read_ut_message_count>
SpikePackToChip<NumPack>::encode_read(coordinate_type const& /*coord*/)
{
	return {};
}

template <size_t NumPack>
std::array<
    hxcomm::vx::UTMessageToFPGAVariant,
    SpikePackToChip<NumPack>::encode_write_ut_message_count>
SpikePackToChip<NumPack>::encode_write(coordinate_type const& /*coord*/) const
{
	std::array<hxcomm::vx::instruction::event_to_fpga::Spike, NumPack> spikes;
	std::transform(m_labels.cbegin(), m_labels.cend(), spikes.begin(), [](auto label) {
		return typename hxcomm::vx::instruction::event_to_fpga::Spike(
		    label.get_neuron_label(), label.get_spl1_address());
	});

	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>>(
	    typename hxcomm::vx::instruction::event_to_fpga::SpikePack<NumPack>::Payload(spikes))};
}

template <size_t NumPack>
void SpikePackToChip<NumPack>::decode(
    std::array<hxcomm::vx::UTMessageFromFPGAVariant, decode_ut_message_count> const& /*messages*/)
{}

template <size_t NumPack>
template <typename Archive>
void SpikePackToChip<NumPack>::serialize(Archive& ar)
{
	ar(CEREAL_NVP(m_labels));
}

template class SpikePackToChip<1>;
template class SpikePackToChip<2>;
template class SpikePackToChip<3>;

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<1>)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<2>)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<3>)


SpikeFromChip::SpikeFromChip() : m_label(), m_chip_time() {}

SpikeFromChip::SpikeFromChip(SpikeLabel const& label, ChipTime const& time) :
    m_label(label),
    m_chip_time(time)
{}

SpikeLabel SpikeFromChip::get_label() const
{
	return m_label;
}

void SpikeFromChip::set_label(SpikeLabel const& value)
{
	m_label = value;
}

ChipTime SpikeFromChip::get_chip_time() const
{
	return m_chip_time;
}

void SpikeFromChip::set_chip_time(ChipTime const& value)
{
	m_chip_time = value;
}

std::ostream& operator<<(std::ostream& os, SpikeFromChip const& spike_event)
{
	std::stringstream ss;
	os << "SpikeFromChip(" << spike_event.m_label << ", " << spike_event.m_chip_time << ")";
	return os;
}

bool SpikeFromChip::operator==(SpikeFromChip const& other) const
{
	return (m_label == other.m_label) && (m_chip_time == other.m_chip_time);
}

bool SpikeFromChip::operator!=(SpikeFromChip const& other) const
{
	return !(*this == other);
}

template <class Archive>
void SpikeFromChip::serialize(Archive& ar)
{
	ar(CEREAL_NVP(m_label));
	ar(CEREAL_NVP(m_chip_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikeFromChip)


MADCSampleFromChip::MADCSampleFromChip() : m_value(), m_chip_time() {}

MADCSampleFromChip::MADCSampleFromChip(Value const& value, ChipTime const& time) :
    m_value(value),
    m_chip_time(time)
{}

MADCSampleFromChip::Value MADCSampleFromChip::get_value() const
{
	return m_value;
}

void MADCSampleFromChip::set_value(Value const& value)
{
	m_value = value;
}

ChipTime MADCSampleFromChip::get_chip_time() const
{
	return m_chip_time;
}

void MADCSampleFromChip::set_chip_time(ChipTime const& value)
{
	m_chip_time = value;
}

std::ostream& operator<<(std::ostream& os, MADCSampleFromChip const& event)
{
	std::stringstream ss;
	os << "MADCSampleFromChip(" << event.m_value << ", " << event.m_chip_time << ")";
	return os;
}

bool MADCSampleFromChip::operator==(MADCSampleFromChip const& other) const
{
	return (m_value == other.m_value) && (m_chip_time == other.m_chip_time);
}

bool MADCSampleFromChip::operator!=(MADCSampleFromChip const& other) const
{
	return !(*this == other);
}

template <class Archive>
void MADCSampleFromChip::serialize(Archive& ar)
{
	ar(CEREAL_NVP(m_value));
	ar(CEREAL_NVP(m_chip_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(MADCSampleFromChip)


SpikeFromChip SpikeFromChipEvent::get_spike() const
{
	return m_spike;
}

void SpikeFromChipEvent::set_spike(SpikeFromChip const& value)
{
	m_spike = value;
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
	os << "SpikeFromChipEvent(" << event.m_spike << ", " << event.m_fpga_time << ")";
	return os;
}

bool SpikeFromChipEvent::operator==(SpikeFromChipEvent const& other) const
{
	return (m_spike == other.m_spike) && (m_fpga_time == other.m_fpga_time);
}

bool SpikeFromChipEvent::operator!=(SpikeFromChipEvent const& other) const
{
	return !(*this == other);
}

template <class Archive>
void SpikeFromChipEvent::serialize(Archive& ar)
{
	ar(CEREAL_NVP(m_spike));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikeFromChipEvent)


MADCSampleFromChip MADCSampleFromChipEvent::get_sample() const
{
	return m_sample;
}

void MADCSampleFromChipEvent::set_sample(MADCSampleFromChip const& value)
{
	m_sample = value;
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
	os << "MADCSampleFromChipEvent(" << event.m_sample << ", " << event.m_fpga_time << ")";
	return os;
}

bool MADCSampleFromChipEvent::operator==(MADCSampleFromChipEvent const& other) const
{
	return (m_sample == other.m_sample) && (m_fpga_time == other.m_fpga_time);
}

bool MADCSampleFromChipEvent::operator!=(MADCSampleFromChipEvent const& other) const
{
	return !(*this == other);
}

template <class Archive>
void MADCSampleFromChipEvent::serialize(Archive& ar)
{
	ar(CEREAL_NVP(m_sample));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(MADCSampleFromChipEvent)

} // namespace fisch::vx
