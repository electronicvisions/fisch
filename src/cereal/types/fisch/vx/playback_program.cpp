#include "fisch/vx/playback_program.h"

#include "fisch/cerealization.tcc"
#include "fisch/vx/detail/playback_program_impl.h"
#include <cereal/types/memory.hpp>

namespace fisch::vx {

template <typename Archive>
void PlaybackProgram::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_impl));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PlaybackProgram)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::PlaybackProgram, 3)
