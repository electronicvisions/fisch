#pragma once

#ifndef FISCH_VX_CHIP_VERSION
#error "Requires FISCH_VX_CHIP_VERSION"
#endif

namespace fisch::vx::detail {

#if FISCH_VX_CHIP_VERSION < 3
constexpr static int jtag_pll_register_coord_size = 3;
#else
constexpr static int jtag_pll_register_coord_size = 4;
#endif
;

} // namespace fisch::vx:detail
