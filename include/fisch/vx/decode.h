#pragma once
#include <array>
#include <boost/range/sub_range.hpp>

#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

typedef boost::sub_range<std::vector<
    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::jtag_from_hicann::Data>> const>
    UTMessageFromFPGARangeJTAG;

typedef boost::sub_range<std::vector<
    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>> const>
    UTMessageFromFPGARangeOmnibus;

} // namespace fisch::vx
