#pragma once
#include "hxcomm/vx/arqconnection.h"
#include "hxcomm/vx/simconnection.h"

#include "fisch/vx/genpybind.h"
#include "fisch/vx/playback_program.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

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
	 * Transfer and execute the given playback program and fetch results.
	 * @param program PlaybackProgram to run
	 */
	void run(std::shared_ptr<PlaybackProgram> const& program);

	/**
	 * Transfer and execute the given messages and return responses.
	 * @param messages UT messages to transfer
	 * @return Response UT messages
	 */
	std::vector<PlaybackProgram::receive_message_type> run(
	    std::vector<PlaybackProgram::send_message_type> const& messages);

private:
	Connection m_connection;
};

template class fisch::vx::PlaybackProgramExecutor<hxcomm::vx::SimConnection>;
template fisch::vx::PlaybackProgramExecutor<hxcomm::vx::SimConnection>::PlaybackProgramExecutor(
    hxcomm::vx::SimConnection::ip_t, hxcomm::vx::SimConnection::port_t);

template class fisch::vx::PlaybackProgramExecutor<hxcomm::vx::ARQConnection>;
template fisch::vx::PlaybackProgramExecutor<hxcomm::vx::ARQConnection>::PlaybackProgramExecutor(
    hxcomm::vx::ARQConnection::ip_t);

#ifdef __GENPYBIND__
typedef fisch::vx::PlaybackProgramExecutor<hxcomm::vx::SimConnection> PlaybackProgramSimExecutor
    GENPYBIND(opaque);
typedef fisch::vx::PlaybackProgramExecutor<hxcomm::vx::ARQConnection> PlaybackProgramARQExecutor
    GENPYBIND(opaque);
#endif // __GENPYBIND__

} // namespace fisch::vx
