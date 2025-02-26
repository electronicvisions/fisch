#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {

/** Systime type for FPGA executor times. */
struct GENPYBIND(inline_base("*")) FPGATime
    : public halco::common::detail::RantWrapper<FPGATime, uint64_t, 0x7ffffffffff, 0>
{
	constexpr explicit FPGATime(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

} // namespace vx
} // namespace fisch

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::FPGATime)

} // namespace std
