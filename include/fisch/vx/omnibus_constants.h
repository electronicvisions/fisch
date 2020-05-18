#pragma once
#include "fisch/vx/omnibus_data.h"
#include "halco/hicann-dls/vx/omnibus.h"

namespace fisch::vx {

constexpr halco::hicann_dls::vx::OmnibusAddress fpga_omnibus_mask{0x8000'0000};
constexpr halco::hicann_dls::vx::OmnibusAddress ut_omnibus_mask{0x0400'0000 | fpga_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress phy_omnibus_mask{0x0200'0000 | ut_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress spi_over_omnibus_mask{0x0100'0000 |
                                                                      phy_omnibus_mask};
constexpr OmnibusData spi_over_omnibus_stop_bit{0x8000'0000};

} // namespace fisch::vx
