#include "fisch/vx/spi.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void SPIShiftRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIShiftRegister)

template <class Archive>
void SPIDACDataRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIDACDataRegister)

template <class Archive>
void SPIDACControlRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIDACControlRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::SPIShiftRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::SPIDACDataRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::SPIDACControlRegister, 0)
