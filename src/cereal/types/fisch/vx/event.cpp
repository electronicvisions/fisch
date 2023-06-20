#include "fisch/vx/event.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"
#include <cereal/types/array.hpp>

namespace fisch::vx {

template <size_t NumPack>
template <typename Archive>
void SpikePackToChip<NumPack>::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<1>)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<2>)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikePackToChip<3>)

template <class Archive>
void SpikeFromChip::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP_("m_label", label));
	ar(CEREAL_NVP_("m_chip_time", chip_time));
	ar(CEREAL_NVP_("m_fpga_time", fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SpikeFromChip)

template <class Archive>
void MADCSampleFromChip::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP_("m_value", value));
	ar(CEREAL_NVP_("m_channel", channel));
	ar(CEREAL_NVP_("m_chip_time", chip_time));
	ar(CEREAL_NVP_("m_fpga_time", fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(MADCSampleFromChip)

template <class Archive>
void HighspeedLinkNotification::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
	ar(CEREAL_NVP(m_fpga_time));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(HighspeedLinkNotification)

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
CEREAL_CLASS_VERSION(fisch::vx::SpikeFromChip, 1)
CEREAL_CLASS_VERSION(fisch::vx::MADCSampleFromChip, 1)
CEREAL_CLASS_VERSION(fisch::vx::HighspeedLinkNotification, 0)
CEREAL_CLASS_VERSION(fisch::vx::TimeoutNotification, 0)
