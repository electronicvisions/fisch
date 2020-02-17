#include "hxcomm/vx/arqconnection.h"
#include "hxcomm/vx/simconnection.h"

#include "fisch/vx/detail/playback_executor.tcc"
#include "fisch/vx/playback_executor.h"

namespace fisch::vx {

template class detail::PlaybackProgramExecutor<hxcomm::vx::ARQConnection>;
template class detail::PlaybackProgramExecutor<hxcomm::vx::SimConnection>;

PlaybackProgramARQExecutor::PlaybackProgramARQExecutor() : base_t() {}

PlaybackProgramARQExecutor::PlaybackProgramARQExecutor(ip_t const ip) : base_t(ip) {}

PlaybackProgramSimExecutor::PlaybackProgramSimExecutor() : base_t() {}

PlaybackProgramSimExecutor::PlaybackProgramSimExecutor(ip_t const ip, port_t const port) :
    base_t(ip, port)
{}

void PlaybackProgramSimExecutor::set_enable_terminate_on_destruction(bool value)
{
	m_connection->set_enable_terminate_on_destruction(value);
}

bool PlaybackProgramSimExecutor::get_enable_terminate_on_destruction() const
{
	return m_connection->get_enable_terminate_on_destruction();
}

} // namespace fisch::vx
