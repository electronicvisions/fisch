#pragma once

#include "fisch/vx/playback_program.h"

// NOTE: In order to seperate dependencies, the implementation in
// fisch/vx/detail/playback_executor.tcc needs to be included seperately!

namespace fisch::vx::detail {

/**
 * Executor base class for playback program.
 * @tparam Connection Connection type
 */
template <class Connection>
class PlaybackProgramExecutor
{
public:
	/**
	 * Create executor with connection specific arguments.
	 * @tparam Args Arguments used for creation of connection
	 */
	template <class... Args>
	PlaybackProgramExecutor(Args&&... args);

	/**
	 * Destructor needs to be implemented where Connection is defined.
	 */
	~PlaybackProgramExecutor();

	/**
	 * Transfer and execute the given playback program and fetch results.
	 * @param program PlaybackProgram to run
	 */
	void run(std::shared_ptr<PlaybackProgram> const& program);

protected:
	std::unique_ptr<Connection> m_connection;
};

} // namespace fisch::vx::detail
