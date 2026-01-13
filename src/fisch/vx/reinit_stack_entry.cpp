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
    std::vector<std::shared_ptr<PlaybackProgram>> const& pbmem_requests,
    std::optional<std::vector<std::shared_ptr<PlaybackProgram>>> const& pbmem_snapshots,
    bool enforce)
{
	if (!m_impl) {
		throw std::runtime_error("Unexpected access to moved-from object.");
	}
	assert(std::all_of(pbmem_requests.begin(), pbmem_requests.end(), [](auto const& request) {
		return static_cast<bool>(request);
	}));
	assert(std::all_of(pbmem_requests.begin(), pbmem_requests.end(), [](auto const& request) {
		return static_cast<bool>(request->m_impl);
	}));
	// Check for each connection that only write-only programs are set.
	if (std::any_of(pbmem_requests.begin(), pbmem_requests.end(), [](auto const& request) {
		    return std::any_of(
		        request->m_impl->m_queue_expected_size.begin(),
		        request->m_impl->m_queue_expected_size.end(), [](auto const& a) { return a > 0; });
	    })) {
		throw std::runtime_error(
		    "Reinit stack entry request can only be set to write-only playback programs.");
	}

	// Build vector over connections of reinit requests for hxcommm ReinitEntry
	std::vector<std::remove_cvref_t<decltype(pbmem_requests.at(0)->get_to_fpga_messages())>>
	    pbmem_requests_impl;
	for (auto const& request : pbmem_requests) {
		pbmem_requests_impl.push_back(request->get_to_fpga_messages());
	}

	if (pbmem_snapshots) {
		assert(std::all_of(
		    pbmem_snapshots->begin(), pbmem_snapshots->end(),
		    [](auto const& snapshot) { return static_cast<bool>(snapshot); }));
		assert(std::all_of(
		    pbmem_snapshots->begin(), pbmem_snapshots->end(),
		    [](auto const& snapshot) { return static_cast<bool>(snapshot->m_impl); }));
		// Build vector over connections of reinit snapshots for hxcommm ReinitEntry
		std::vector<std::remove_cvref_t<decltype(pbmem_snapshots->at(0)->get_to_fpga_messages())>>
		    pbmem_snapshots_impl;
		for (auto const& snapshot : *pbmem_snapshots) {
			pbmem_snapshots_impl.push_back(snapshot->get_to_fpga_messages());
		}

		m_impl->set({std::move(pbmem_requests_impl), std::move(pbmem_snapshots_impl), enforce});
	} else {
		m_impl->set({std::move(pbmem_requests_impl), std::nullopt, enforce});
	}
}

void ReinitStackEntry::set(
    std::vector<std::shared_ptr<PlaybackProgram>>&& pbmem_requests,
    std::optional<std::vector<std::shared_ptr<PlaybackProgram>>>&& pbmem_snapshots,
    bool enforce)
{
	if (!m_impl) {
		throw std::runtime_error("Unexpected access to moved-from object.");
	}
	assert(std::all_of(
	    pbmem_requests.begin(), pbmem_requests.end(),
	    [](auto const& request) -> auto& { return request; }));
	assert(std::all_of(
	    pbmem_requests.begin(), pbmem_requests.end(),
	    [](auto const& request) -> auto& { return request->m_impl; }));
	if (std::any_of(pbmem_requests.begin(), pbmem_requests.end(), [](auto const& request) {
		    return std::any_of(
		        request->m_impl->m_queue_expected_size.begin(),
		        request->m_impl->m_queue_expected_size.end(), [](auto const& a) { return a > 0; });
	    })) {
		throw std::runtime_error(
		    "Reinit stack entry request can only be set to write-only playback programs.");
	}

	auto moved_to_fpga_messages =
	    [&pbmem_requests](std::vector<std::shared_ptr<PlaybackProgram>>& programs) {
		    std::vector<std::remove_cvref_t<decltype(pbmem_requests.at(0)->get_to_fpga_messages())>>
		        to_fpga_messages;
		    to_fpga_messages.reserve(programs.size());
		    std::transform(
		        programs.begin(), programs.end(), std::back_inserter(to_fpga_messages),
		        [](std::shared_ptr<PlaybackProgram>& program) {
			        return std::move(program->m_impl->m_instructions);
		        });
		    return to_fpga_messages;
	    };

	if (pbmem_snapshots) {
		assert(std::all_of(
		    pbmem_snapshots.value().begin(), pbmem_snapshots.value().end(),
		    [](auto const& snapshot) -> auto& { return snapshot; }));
		assert(std::all_of(
		    pbmem_snapshots.value().begin(), pbmem_snapshots.value().end(),
		    [](auto const& snapshot) -> auto& { return snapshot->m_impl; }));
		m_impl->set(
		    {moved_to_fpga_messages(pbmem_requests), moved_to_fpga_messages(*pbmem_snapshots),
		     enforce});
	} else {
		m_impl->set({moved_to_fpga_messages(pbmem_requests), std::nullopt, enforce});
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
