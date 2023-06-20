#include "fisch/vx/omnibus.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void Omnibus::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Omnibus)

template <class Archive>
void PollingOmnibusBlock::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PollingOmnibusBlock)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Omnibus, 1)
CEREAL_CLASS_VERSION(fisch::vx::PollingOmnibusBlock, 1)
