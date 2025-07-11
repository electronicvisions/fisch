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
	std::vector<std::reference_wrapper<std::vector<PlaybackProgram::to_fpga_message_type> const>>
	    fpga_messages;
	fpga_messages.push_back(program->get_to_fpga_messages());

	auto result = hxcomm::execute_messages(connection, fpga_messages);

	std::vector<typename decltype(result)::value_type::second_type> connection_time_info;

	for (auto single_result : result) {
		connection_time_info.push_back(single_result.second);
	}

	hate::Timer timer;
	program->clear_from_fpga_messages();

	if constexpr (hate::is_container_v<decltype(result)>) {
		for (auto const& response : result.at(0).first) { // TO-DO: Correct vectorization
			program->push_from_fpga_message(response);
		}
	} else {
		while (!result.empty()) {
			program->push_from_fpga_message(result.at(0).first); // TO-DO Correct vectorization
			result.at(0).pop();                                  // TO-DO Correct vectorization
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
