#pragma once
#include "fisch/vx/genpybind.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(visible) NullPayloadReadable
{
	bool operator==(NullPayloadReadable const& other) const;
	bool operator!=(NullPayloadReadable const& other) const;
};

} // namespace word_access_type
} // namespace fisch::vx
