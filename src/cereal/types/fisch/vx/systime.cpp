#include "fisch/vx/systime.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void SystimeSync::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SystimeSync)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::SystimeSync, 1)
