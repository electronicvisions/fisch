#pragma once
#include "fisch/vx/omnibus_data.h"
#include "halco/hicann-dls/vx/coordinates.h"

namespace fisch::vx {

constexpr halco::hicann_dls::vx::OmnibusAddress spi_over_omnibus_mask{0x01000000};
constexpr OmnibusData spi_over_omnibus_stop_bit{0x80000000};

} // namespace fisch::vx
