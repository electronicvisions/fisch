#pragma once
#include "fisch/vx/back_emplacer_fwd.h"
#include "hxcomm/vx/utmessage_fwd.h"
#include <vector>

namespace fisch::vx {

typedef BackEmplacer<std::vector<hxcomm::vx::UTMessageToFPGAVariant>> UTMessageToFPGABackEmplacer;

} // namespace fisch::vx
