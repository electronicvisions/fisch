#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(visible) Timer
{
	bool operator==(Timer const& other) const;
	bool operator!=(Timer const& other) const;
};

struct GENPYBIND(inline_base("*")) WaitUntil
    : public halco::common::detail::BaseType<WaitUntil, uint32_t>
{
	constexpr explicit WaitUntil(uintmax_t value = 0) GENPYBIND(implicit_conversion) : base_t(value)
	{}
};

} // namespace word_access_type
} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::WaitUntil)
} // namespace std
