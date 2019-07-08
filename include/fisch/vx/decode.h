#pragma once
#include <vector>
#include <boost/range/sub_range.hpp>

#include "hxcomm/vx/utmessage_fwd.h"

namespace fisch::vx {

template <typename Instruction>
using UTMessageFromFPGARange =
    boost::sub_range<std::vector<hxcomm::vx::UTMessageFromFPGA<Instruction>> const>;


typedef UTMessageFromFPGARange<hxcomm::vx::instruction::jtag_from_hicann::Data>
    UTMessageFromFPGARangeJTAG;

typedef UTMessageFromFPGARange<hxcomm::vx::instruction::omnibus_from_fpga::Data>
    UTMessageFromFPGARangeOmnibus;

typedef UTMessageFromFPGARange<hxcomm::vx::instruction::from_fpga_system::Loopback>
    UTMessageFromFPGARangeLoopback;

} // namespace fisch::vx
