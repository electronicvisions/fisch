#pragma once

#include "fisch/vx/detail/execute_words.h"
#include "fisch/vx/detail/playback_executor.h"

namespace fisch::vx::detail {

template <class Connection>
template <class... Args>
PlaybackProgramExecutor<Connection>::PlaybackProgramExecutor(Args&&... args) :
    m_connection(new Connection(std::forward<Args>(args)...))
{}

template <typename Connection>
PlaybackProgramExecutor<Connection>::~PlaybackProgramExecutor() = default;

template <class Connection>
void PlaybackProgramExecutor<Connection>::run(std::shared_ptr<PlaybackProgram> const& program)
{
	auto responses = execute_words(*m_connection, program->get_to_fpga_messages());
	program->clear_from_fpga_messages();
	for (auto const& message : responses) {
		program->push_from_fpga_message(message);
	}
	if (!program->valid()) {
		throw std::runtime_error("Not all response data to PlaybackProgram valid.");
	}
}

} // namespace fisch::vx::detail
