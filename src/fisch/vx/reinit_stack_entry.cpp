#include "fisch/vx/reinit_stack_entry.h"

#include "hxcomm/vx/reinit_stack_entry.h"
#include <algorithm>
#include <stdexcept>

namespace fisch::vx {

ReinitStackEntry::~ReinitStackEntry()
{
	/* needs to be specified here because Impl not complete in inline-default definition */
}

void ReinitStackEntry::set(std::shared_ptr<PlaybackProgram> const& pbmem, bool enforce)
{
	if (!m_impl) {
		throw std::runtime_error("Unexpected access to moved-from object.");
	}
	assert(pbmem);
	if (std::any_of(
	        pbmem->m_queue_expected_size.begin(), pbmem->m_queue_expected_size.end(),
	        [](auto const& a) { return a > 0; })) {
		throw std::runtime_error(
		    "Reinit stack entry can only be set to write-only playback programs.");
	}
	m_impl->set(pbmem->get_to_fpga_messages(), enforce);
}

void ReinitStackEntry::enforce()
{
	if (!m_impl) {
		throw std::runtime_error("Unexpected access to moved-from object.");
	}
	m_impl->enforce();
}

void ReinitStackEntry::pop()
{
	if (!m_impl) {
		throw std::runtime_error("Unexpected access to moved-from object.");
	}
	m_impl->pop();
}

} // namespace fisch::vx
