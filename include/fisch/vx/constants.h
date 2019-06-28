#pragma once
#include "hxcomm/vx/instruction/timing.h"

#include "fisch/vx/genpybind.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

typedef hxcomm::vx::instruction::timing::WaitUntil::value_type hardware_timer_value_type;

constexpr hardware_timer_value_type GENPYBIND(visible) fpga_clock_cycles_per_us = 125;

} // namespace vx
} // namespace fisch
