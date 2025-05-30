#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) Timer : public halco::common::detail::BaseType<Timer, uint32_t>
{
	constexpr explicit Timer(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) WaitUntil
    : public halco::common::detail::BaseType<WaitUntil, uint32_t>
{
	constexpr explicit WaitUntil(uintmax_t value = 0) GENPYBIND(implicit_conversion) : base_t(value)
	{}
};

} // namespace word_access_type
} // namespace vx
} // namespace fisch

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::WaitUntil)
} // namespace std
