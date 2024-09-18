#include "fisch/vx/playback_program.h"
#include "fisch/vx/run_time_info.h"
#include "hate/timer.h"
#include "hate/type_traits.h"
#include "hxcomm/common/execute_messages.h"
#include <chrono>

namespace fisch::vx {

template <typename Connection>
RunTimeInfo run(Connection& connection, std::shared_ptr<PlaybackProgram> const& program)
{
	if (!program) {
		throw std::runtime_error("run() of invalid shared_ptr<PlaybackProgram> not possible.");
	}
	auto [responses, connection_time_info] =
	    hxcomm::execute_messages(connection, program->get_to_fpga_messages());
	hate::Timer timer;
	program->clear_from_fpga_messages();
	if constexpr (hate::is_container_v<decltype(responses)>) {
		for (auto const& response : responses) {
			program->push_from_fpga_message(response);
		}
	} else {
		while (!responses.empty()) {
			program->push_from_fpga_message(responses.front());
			responses.pop();
		}
	}
	if (!program->run_ok()) {
		throw std::runtime_error("FPGA sent error notification(s). Please check"
			" for previous error messages.");
	}
	if (!program->tickets_valid()) {
		throw std::runtime_error("Not all response data to PlaybackProgram valid.");
	}
	return {connection_time_info, std::chrono::nanoseconds(timer.get_ns())};
}

} // namespace fisch::vx
