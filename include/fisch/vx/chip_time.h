#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/** Systime type for chip annotated times. */
struct GENPYBIND(inline_base("*")) ChipTime
    : public halco::common::detail::RantWrapper<ChipTime, uint64_t, 0x7ffffffffff, 0>
{
	constexpr explicit ChipTime(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::ChipTime)

} // namespace std
