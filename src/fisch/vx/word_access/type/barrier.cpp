#include "fisch/vx/word_access/type/barrier.h"

#include "hxcomm/vx/instruction/timing.h"

namespace fisch::vx::word_access_type {

Barrier const Barrier::jtag{static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::jtag)};
Barrier const Barrier::omnibus{
    static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::omnibus)};
Barrier const Barrier::systime{
    static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::systime)};
Barrier const Barrier::multi_fpga{
    static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::multi_fpga)};
Barrier const Barrier::systime_correction{
    static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::systime_correction)};

} // namespace fisch::vx::word_access_type
