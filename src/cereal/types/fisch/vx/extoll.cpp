#include "fisch/vx/extoll.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void Extoll::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Extoll)

template <class Archive>
void ExtollOnNwNode::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ExtollOnNwNode)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Extoll, 0)
CEREAL_CLASS_VERSION(fisch::vx::ExtollOnNwNode, 0)
