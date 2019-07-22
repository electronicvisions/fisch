#include "fisch/vx/playback_executor.h"

namespace fisch::vx {

template <class Connection>
template<class... Args>
PlaybackProgramExecutor<Connection>::PlaybackProgramExecutor(Args... args) : m_connection(args...)
{}

template <class Connection>
void PlaybackProgramExecutor<Connection>::run(std::shared_ptr<PlaybackProgram> const& program)
{
	m_connection.add(program->get_to_fpga_messages());
	m_connection.add(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Halt>());
	m_connection.commit();

	m_connection.run_until_halt();

	program->clear_from_fpga_messages();

	typename PlaybackProgram::from_fpga_message_type message;
	while (m_connection.try_receive(message)) {
		program->push_from_fpga_message(message);
	}
}

template <class Connection>
std::vector<PlaybackProgram::from_fpga_message_type> PlaybackProgramExecutor<Connection>::run(
    std::vector<PlaybackProgram::to_fpga_message_type> const& messages)
{
	m_connection.add(messages);
	m_connection.add(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Halt>());
	m_connection.commit();

	m_connection.run_until_halt();

	std::vector<PlaybackProgram::from_fpga_message_type> responses;
	typename PlaybackProgram::from_fpga_message_type response;
	while (m_connection.try_receive(response)) {
		responses.push_back(response);
	}
	return responses;
}

template class PlaybackProgramExecutor<hxcomm::vx::ARQConnection>;
template class PlaybackProgramExecutor<hxcomm::vx::SimConnection>;

PlaybackProgramARQExecutor::PlaybackProgramARQExecutor() : base_t() {}

PlaybackProgramARQExecutor::PlaybackProgramARQExecutor(ip_t const ip) : base_t(ip) {}

PlaybackProgramSimExecutor::PlaybackProgramSimExecutor(ip_t const ip, port_t const port) : base_t(ip, port) {}

void PlaybackProgramSimExecutor::set_enable_terminate_on_destruction(bool const value)
{
	m_connection.set_enable_terminate_on_destruction(value);
}

bool PlaybackProgramSimExecutor::get_enable_terminate_on_destruction() const
{
	return m_connection.get_enable_terminate_on_destruction();
}

} // namespace fisch::vx
