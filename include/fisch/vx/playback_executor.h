#pragma once
#include "fisch/vx/playback_program.h"

namespace fisch::vx {

/**
 * Executor for playback program.
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
	PlaybackProgramExecutor(Args... args) : m_connection(args...)
	{}

	/**
	 * Transfer vector of UT messages to connection.
	 * @param messages UT messages to transfer
	 */
	void transfer(std::vector<PlaybackProgram::send_message_type> const& messages);

	/**
	 * Transfer playback program instruction data to connection.
	 * @param program Playback program to transfer
	 */
	void transfer(std::shared_ptr<PlaybackProgram> const& program);

	/**
	 * Execute transferred playback program until a halt response is decoded.
	 */
	void execute();

	/**
	 * Fetch playback program results of execution.
	 * @param program Playback program to insert results into
	 */
	void fetch(std::shared_ptr<PlaybackProgram> const& program);

	/**
	 * Fetch vector of UT messages from connection.
	 * @return UT message container to place fetched messages into
	 */
	std::vector<PlaybackProgram::receive_message_type> fetch();

private:
	Connection m_connection;
};

} // namespace fisch::vx
