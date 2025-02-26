#pragma once
#include "fisch/vx/genpybind.h"
#include "hate/visibility.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(visible) NullPayloadReadable
{
	bool operator==(NullPayloadReadable const& other) const SYMBOL_VISIBLE;
	bool operator!=(NullPayloadReadable const& other) const SYMBOL_VISIBLE;
};

} // namespace word_access_type
} // namespace vx
} // namespace fisch
