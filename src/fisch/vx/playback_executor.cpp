#include "fisch/vx/playback_executor.h"

namespace fisch::vx {

template <class Connection>
void PlaybackProgramExecutor<Connection>::run(std::shared_ptr<PlaybackProgram> const& program)
{
	if (program->get_to_fpga_messages().empty()) {
		throw std::logic_error("Trying to run empty PlaybackProgram.");
	}

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
	if (messages.empty()) {
		throw std::logic_error("Trying to run empty UT message sequence.");
	}

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

} // namespace fisch::vx
