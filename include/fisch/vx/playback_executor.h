#pragma once
#include "hxcomm/vx/arqconnection.h"
#include "hxcomm/vx/simconnection.h"

#include "fisch/vx/genpybind.h"
#include "fisch/vx/playback_program.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

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
	PlaybackProgramExecutor(Args... args);

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
	std::vector<PlaybackProgram::from_fpga_message_type> run(
	    std::vector<PlaybackProgram::to_fpga_message_type> const& messages) GENPYBIND(hidden);

protected:
	Connection m_connection;
};


/**
 * Executor executing with an ARQConnection.
 */
class GENPYBIND(inline_base("*")) PlaybackProgramARQExecutor
    : public PlaybackProgramExecutor<hxcomm::vx::ARQConnection>
{
public:
	typedef PlaybackProgramExecutor<hxcomm::vx::ARQConnection> base_t;
	typedef hxcomm::vx::ARQConnection::ip_t ip_t;

	/**
	 * Construct ARQ executor with IP address extracted from env.
	 * @throws std::runtime_error On no or more than one IP address available in environment
	 */
	PlaybackProgramARQExecutor();

	/**
	 * Construct ARQ executor to FPGA with given IP address.
	 * @param ip IP address to use
	 */
	PlaybackProgramARQExecutor(ip_t ip);
};


/**
 * Executor executing with a SimConnection.
 */
class GENPYBIND(inline_base("*")) PlaybackProgramSimExecutor
    : public PlaybackProgramExecutor<hxcomm::vx::SimConnection>
{
public:
	typedef PlaybackProgramExecutor<hxcomm::vx::SimConnection> base_t;
	typedef hxcomm::vx::SimConnection::ip_t ip_t;
	typedef hxcomm::vx::SimConnection::port_t port_t;

	/**
	 * Construct Simulation executor with port extracted from env.
	 * @throws std::runtime_error On no port available in environment
	 */
	PlaybackProgramSimExecutor();

	/**
	 * Construct Simulation executor with given IP address and port.
	 * @param ip IP address to use
	 * @param port Port to use
	 */
	PlaybackProgramSimExecutor(ip_t ip, port_t port);

	/**
	 * Set enable value to terminate simulator on destruction of executor.
	 * @param value Boolean value
	 */
	GENPYBIND(setter_for(enable_terminate_on_destruction))
	void set_enable_terminate_on_destruction(bool value);

	/**
	 * Get enable value to terminate simulator on destruction of executor.
	 * @return Boolean value
	 */
	GENPYBIND(getter_for(enable_terminate_on_destruction))
	bool get_enable_terminate_on_destruction() const;
};

} // namespace fisch::vx
