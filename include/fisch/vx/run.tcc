
#include "fisch/vx/playback_program.h"

#include "hxcomm/common/execute_messages.h"

namespace fisch::vx {

template <typename Connection>
void run(Connection& connection, std::shared_ptr<PlaybackProgram> const& program)
{
	auto responses = hxcomm::execute_messages(connection, program->get_to_fpga_messages());
	program->clear_from_fpga_messages();
	for (auto const& message : responses) {
		program->push_from_fpga_message(message);
	}
	if (!program->valid()) {
		throw std::runtime_error("Not all response data to PlaybackProgram valid.");
	}
}

} // namespace fisch::vx
