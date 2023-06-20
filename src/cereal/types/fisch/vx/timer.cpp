#include "fisch/vx/timer.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void Timer::serialize(Archive&, std::uint32_t const)
{}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Timer)

template <class Archive>
void WaitUntil::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(WaitUntil)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Timer, 1)
CEREAL_CLASS_VERSION(fisch::vx::WaitUntil, 0)
