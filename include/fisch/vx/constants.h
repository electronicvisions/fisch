#pragma once
#include <stddef.h>
#include "fisch/vx/genpybind.h"

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

/** Number of FPGA clock cycles per {micro,milli,}second. */
constexpr size_t GENPYBIND(visible) fpga_clock_cycles_per_us = 125;
constexpr size_t GENPYBIND(visible) fpga_clock_cycles_per_ms = fpga_clock_cycles_per_us * 1000;
constexpr size_t GENPYBIND(visible) fpga_clock_cycles_per_s = fpga_clock_cycles_per_ms * 1000;

/** Maximal JTAG clock-sclaer value. */
constexpr size_t jtag_clock_scaler_max = 0xff;

/** Minimal JTAG clock-sclaer value. */
constexpr size_t jtag_clock_scaler_min = 0;

/** Maximal xboard DAC channel value. */
constexpr size_t dac_value_max = 0xfff;

/** Minimal xboard DAC channel value. */
constexpr size_t dac_value_min = 0;

/** Memory size on FPGA in instructions for playback execution. */
constexpr size_t playback_memory_size_to_fpga = 128768;

} // namespace vx
} // namespace fisch
