#pragma once
#include "fisch/vx/constants.h"
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) JTAGClockScaler
    : public halco::common::detail::
          RantWrapper<JTAGClockScaler, uint_fast16_t, jtag_clock_scaler_max, jtag_clock_scaler_min>
{
	constexpr explicit JTAGClockScaler(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) OmnibusChipOverJTAG
    : public halco::common::detail::BaseType<OmnibusChipOverJTAG, uint32_t>
{
	constexpr explicit OmnibusChipOverJTAG(value_type const value = 0)
	    GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) JTAGIdCode
    : public halco::common::detail::BaseType<JTAGIdCode, uint32_t>
{
	constexpr explicit JTAGIdCode(value_type const value = 0) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) JTAGPLLRegister
    : public halco::common::detail::BaseType<JTAGPLLRegister, uint32_t>
{
	constexpr explicit JTAGPLLRegister(value_type const value = 0) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) JTAGPhyRegister
    : public halco::common::detail::
          RantWrapper<JTAGPhyRegister, uint_fast16_t, 4194303 /* 2^22-1 */, 0>
{
	constexpr explicit JTAGPhyRegister(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

struct GENPYBIND(visible) ResetJTAGTap
{
	bool operator==(ResetJTAGTap const& other) const;
	bool operator!=(ResetJTAGTap const& other) const;
};

} // namespace word_access_type
} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::JTAGIdCode)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::JTAGPLLRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::JTAGClockScaler)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::JTAGPhyRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::OmnibusChipOverJTAG)

} // namespace std
