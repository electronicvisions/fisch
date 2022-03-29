#include "fisch/vx/reinit_stack_entry.h"

#include "fisch/vx/detail/playback_program_impl.h"
#include "hxcomm/vx/reinit_stack_entry.h"
#include <algorithm>
#include <stdexcept>

namespace fisch::vx {

ReinitStackEntry::~ReinitStackEntry()
{
	/* needs to be specified here because Impl not complete in inline-default definition */
}

void ReinitStackEntry::set(
    std::shared_ptr<PlaybackProgram> const& pbmem_request,
    std::optional<std::shared_ptr<PlaybackProgram>> const& pbmem_snapshot,
    bool enforce)
{
	if (!m_impl) {
		throw std::runtime_error("Unexpected access to moved-from object.");
	}
	assert(pbmem_request);
	assert(pbmem_request->m_impl);
	if (std::any_of(
	        pbmem_request->m_impl->m_queue_expected_size.begin(),
	        pbmem_request->m_impl->m_queue_expected_size.end(),
	        [](auto const& a) { return a > 0; })) {
		throw std::runtime_error(
		    "Reinit stack entry request can only be set to write-only playback programs.");
	}
	if (pbmem_snapshot) {
		assert(*pbmem_snapshot);
		assert((*pbmem_snapshot)->m_impl);
		m_impl->set(
		    {pbmem_request->get_to_fpga_messages(), (*pbmem_snapshot)->get_to_fpga_messages()},
		    enforce);
	} else {
		m_impl->set({pbmem_request->get_to_fpga_messages(), std::nullopt}, enforce);
	}
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
