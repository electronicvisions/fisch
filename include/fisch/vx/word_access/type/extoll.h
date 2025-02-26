#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) Extoll : public halco::common::detail::BaseType<Extoll, uint64_t>
{
	constexpr explicit Extoll(value_type const value = 0) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

struct GENPYBIND(inline_base("*")) ExtollOnNwNode
    : public halco::common::detail::BaseType<ExtollOnNwNode, uint64_t>
{
	constexpr explicit ExtollOnNwNode(value_type const value = 0) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

} // namespace word_access_type
} // namespace vx
} // namespace fisch

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::Extoll)
} // namespace std
