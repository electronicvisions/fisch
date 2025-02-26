#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) SystimeSync
    : public halco::common::detail::BaseType<SystimeSync, bool>
{
	constexpr explicit SystimeSync(bool value = false) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

} // namespace word_access_type
} // namespace vx
} // namespace fisch
