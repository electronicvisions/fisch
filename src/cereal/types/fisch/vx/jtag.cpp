#include "fisch/vx/jtag.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void ResetJTAGTap::serialize(Archive& /*ar*/, std::uint32_t const)
{}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ResetJTAGTap)

template <class Archive>
void JTAGClockScaler::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGClockScaler)

template <class Archive>
void OmnibusChipOverJTAG::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(OmnibusChipOverJTAG)

template <class Archive>
void JTAGIdCode::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGIdCode)

template <class Archive>
void JTAGPLLRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGPLLRegister)

template <class Archive>
void JTAGPhyRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGPhyRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::ResetJTAGTap, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGClockScaler, 0)
CEREAL_CLASS_VERSION(fisch::vx::OmnibusChipOverJTAG, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGIdCode, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGPLLRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGPhyRegister, 0)
