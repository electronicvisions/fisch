#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) Barrier
    : public halco::common::detail::RantWrapper<Barrier, uint_fast32_t, 0x1f, 0>
{
	constexpr explicit Barrier(uintmax_t const value = 0x0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}

	static const Barrier jtag;
	static const Barrier omnibus;
	static const Barrier systime;
	static const Barrier multi_fpga;
	static const Barrier systime_correction;
};

} // namespace word_access_type
} // namespace fisch::vx
