#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"
#include "halco/hicann-dls/vx/event.h"

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

typedef std::array<halco::hicann_dls::vx::SpikeLabel, 1> SpikePack1ToChip;
typedef std::array<halco::hicann_dls::vx::SpikeLabel, 2> SpikePack2ToChip;
typedef std::array<halco::hicann_dls::vx::SpikeLabel, 3> SpikePack3ToChip;

} // namespace word_access_type
} // namespace vx
} // namespace fisch
