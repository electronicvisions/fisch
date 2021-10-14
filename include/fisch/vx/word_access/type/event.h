#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

/**
 * Spike payload information.
 */
struct GENPYBIND(inline_base("*")) SpikeLabel
    : public halco::common::detail::BaseType<SpikeLabel, uint16_t>
{
	constexpr explicit SpikeLabel(value_type const val = 0) : base_t(val) {}
};

typedef std::array<SpikeLabel, 1> SpikePack1ToChip;
typedef std::array<SpikeLabel, 2> SpikePack2ToChip;
typedef std::array<SpikeLabel, 3> SpikePack3ToChip;

} // namespace word_access_type
} // namespace fisch::vx
