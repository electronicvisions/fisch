#include "fisch/vx/word_access/type/omnibus.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"
#include <cereal/types/array.hpp>

namespace fisch::vx::word_access_type {

template <class Archive>
void Omnibus::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(word));
	ar(CEREAL_NVP(byte_enables));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Omnibus)

} // namespace fisch::vx::word_access_type

CEREAL_CLASS_VERSION(fisch::vx::word_access_type::Omnibus, 0)
