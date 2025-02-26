#pragma once
#include "fisch/vx/constants.h"
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) SPIShiftRegister
    : public halco::common::detail::RantWrapper<SPIShiftRegister, uint_fast32_t, 0xffffff, 0>
{
	constexpr explicit SPIShiftRegister(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) SPIDACDataRegister
    : public halco::common::detail::
          RantWrapper<SPIDACDataRegister, uint_fast32_t, dac_value_max, dac_value_min>
{
	constexpr explicit SPIDACDataRegister(uintmax_t const value = 0)
	    GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) SPIDACControlRegister
    : public halco::common::detail::RantWrapper<SPIDACControlRegister, uint_fast32_t, 0x1fff, 0>
{
	constexpr explicit SPIDACControlRegister(uintmax_t const value = 0)
	    GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

} // namespace word_access_type
} // namespace vx
} // namespace fisch

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::SPIShiftRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::SPIDACControlRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::SPIDACDataRegister)

} // namespace std
