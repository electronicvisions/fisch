#pragma once
#include "halco/common/geometry.h"

namespace fisch::vx {

/** Typesafe omnibus data */
struct OmnibusData : public halco::common::detail::BaseType<OmnibusData, uint32_t>
{
	OmnibusData() : base_t(0) {}
	explicit OmnibusData(value_type const value) : base_t(value) {}
};

} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::OmnibusData)
} // namespace std
