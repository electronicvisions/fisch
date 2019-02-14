#pragma once
#include "hxcomm/vx/instruction/timing.h"

namespace fisch::vx {

typedef hxcomm::vx::instruction::timing::wait_until::value_type hardware_timer_value_type;

constexpr hardware_timer_value_type fpga_clock_cycles_per_us = 125;

}
