#pragma once

#include "hxcomm/common/connect_to_remote_parameter_defs.h"

#include "fisch/vx/detail/playback_executor.h"
#include "fisch/vx/genpybind.h"

namespace hxcomm::vx {

class ARQConnection;
class SimConnection;

} // namespace hxcomm::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

extern template class detail::PlaybackProgramExecutor<hxcomm::vx::ARQConnection>;
extern template class detail::PlaybackProgramExecutor<hxcomm::vx::SimConnection>;

/**
 * Executor executing with an ARQConnection.
 *
 * Defined again so that genpybind can instanciate explicit constructors.
 */
class GENPYBIND(inline_base("*")) PlaybackProgramARQExecutor
    : public detail::PlaybackProgramExecutor<hxcomm::vx::ARQConnection>
{
public:
	using base_t = PlaybackProgramExecutor<hxcomm::vx::ARQConnection>;
	using ip_t = hxcomm::ip_t;

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
 *
 * Defined again so that genpybind can instanciate explicit constructors.
 */
class GENPYBIND(inline_base("*")) PlaybackProgramSimExecutor
    : public detail::PlaybackProgramExecutor<hxcomm::vx::SimConnection>
{
public:
	using base_t = PlaybackProgramExecutor<hxcomm::vx::SimConnection>;
	using ip_t = hxcomm::ip_t;
	using port_t = hxcomm::port_t;

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
