#include "fisch/vx/playback_executor.h"

namespace fisch::vx {

template <class Connection>
void PlaybackProgramExecutor<Connection>::run(std::shared_ptr<PlaybackProgram> const& program)
{
	m_connection.add(program->get_send_messages());
	m_connection.commit();

	m_connection.run_until_halt();

	typename PlaybackProgram::receive_message_type message;
	while (m_connection.try_receive(message)) {
		program->push_received_message(message);
	}
}

template <class Connection>
std::vector<PlaybackProgram::receive_message_type> PlaybackProgramExecutor<Connection>::run(
    std::vector<PlaybackProgram::send_message_type> const& messages)
{
	m_connection.add(messages);
	m_connection.commit();

	m_connection.run_until_halt();

	std::vector<PlaybackProgram::receive_message_type> responses;
	typename PlaybackProgram::receive_message_type response;
	while (m_connection.try_receive(response)) {
		responses.push_back(response);
	}
	return responses;
}

} // namespace fisch::vx
