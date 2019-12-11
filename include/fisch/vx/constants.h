#pragma once
#include <stddef.h>
#include "fisch/vx/genpybind.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

/** Number of FPGA clock cycles per microsecond. */
constexpr size_t GENPYBIND(visible) fpga_clock_cycles_per_us = 125;

} // namespace vx
} // namespace fisch
