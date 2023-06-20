#include "fisch/vx/barrier.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void Barrier::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Barrier)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Barrier, 0)
