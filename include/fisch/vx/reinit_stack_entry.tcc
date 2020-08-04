#include "fisch/vx/reinit_stack_entry.h"

#include "hxcomm/vx/reinit_stack_entry.h"

namespace fisch::vx {

// We simply wrap the hxcomm ReinitStack
class ReinitStackEntry::Impl : public hxcomm::vx::ReinitStackEntry
{};

template <typename Connection>
ReinitStackEntry::ReinitStackEntry(Connection& connection) : m_impl(new Impl{connection})
{}

} // namespace fisch::vx
