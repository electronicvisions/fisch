#include "fisch/vx/reset.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void ResetChip::serialize(Archive& ar, std::uint32_t)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ResetChip)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::ResetChip, 1)
