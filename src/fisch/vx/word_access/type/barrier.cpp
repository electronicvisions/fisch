#include "fisch/vx/word_access/type/barrier.h"

#include "hxcomm/vx/instruction/timing.h"

namespace fisch::vx::word_access_type {

Barrier const Barrier::jtag{static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::jtag)};
Barrier const Barrier::omnibus{
    static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::omnibus)};
Barrier const Barrier::systime{
    static_cast<uintmax_t>(hxcomm::vx::instruction::timing::Barrier::systime)};

} // namespace fisch::vx::word_access_type
