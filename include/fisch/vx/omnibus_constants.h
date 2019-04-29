#pragma once
#include "fisch/vx/omnibus_data.h"
#include "halco/hicann-dls/vx/coordinates.h"

namespace fisch::vx {

constexpr halco::hicann_dls::vx::OmnibusFPGAAddress executor_omnibus_mask{0x8000'0000};
constexpr halco::hicann_dls::vx::OmnibusFPGAAddress ut_omnibus_mask{0x0400'0000};
constexpr halco::hicann_dls::vx::OmnibusFPGAAddress phy_omnibus_mask{0x0200'0000 | ut_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusFPGAAddress spi_over_omnibus_mask{0x0100'0000 |
                                                                          phy_omnibus_mask};
constexpr OmnibusData spi_over_omnibus_stop_bit{0x8000'0000};

} // namespace fisch::vx
