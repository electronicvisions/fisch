#include "fisch/vx/playback_executor.h"
#include "hxcomm/vx/arqconnection.h"
#include "hxcomm/vx/simconnection.h"

namespace fisch::vx {

template <class Connection>
void PlaybackProgramExecutor<Connection>::transfer(
    std::vector<PlaybackProgram::send_message_type> const& messages)
{
	m_connection.add(messages);
	m_connection.commit();
}

template <class Connection>
void PlaybackProgramExecutor<Connection>::transfer(std::shared_ptr<PlaybackProgram> const& program)
{
	transfer(program->get_send_messages());
}

template <class Connection>
void PlaybackProgramExecutor<Connection>::execute()
{
	m_connection.run_until_halt();
}

template <class Connection>
void PlaybackProgramExecutor<Connection>::fetch(std::shared_ptr<PlaybackProgram> const& program)
{
	typename PlaybackProgram::receive_message_type message;
	while (m_connection.try_receive(message)) {
		program->push_received_message(message);
	}
}

template <class Connection>
std::vector<PlaybackProgram::receive_message_type> PlaybackProgramExecutor<Connection>::fetch()
{
	std::vector<PlaybackProgram::receive_message_type> messages;
	typename PlaybackProgram::receive_message_type message;
	while (m_connection.try_receive(message)) {
		messages.push_back(message);
	}
	return messages;
}

template class PlaybackProgramExecutor<hxcomm::vx::ARQConnection>;
template class PlaybackProgramExecutor<hxcomm::vx::SimConnection>;

} // namespace fisch::vx
