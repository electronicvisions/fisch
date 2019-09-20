#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/** Typesafe omnibus data */
struct GENPYBIND(inline_base("*")) OmnibusData
    : public halco::common::detail::BaseType<OmnibusData, uint32_t>
{
	constexpr explicit OmnibusData(value_type const value = 0) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::OmnibusData)
} // namespace std
