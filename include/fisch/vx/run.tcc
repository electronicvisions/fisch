#include "fisch/vx/playback_program.h"
#include "fisch/vx/run_time_info.h"
#include "hate/timer.h"
#include "hate/type_traits.h"
#include "hxcomm/common/execute_messages.h"
#include <chrono>
#include <cstddef>

namespace fisch::vx {

template <typename Connection>
RunTimeInfo run(
    Connection& connection, std::vector<std::shared_ptr<PlaybackProgram>> const& programs)
{
	if (hxcomm::visit_connection([](auto& conn) { return conn.size(); }, connection) !=
	    programs.size()) {
		throw std::runtime_error("For each fpga exactly one program is required.");
	}

	std::vector<std::reference_wrapper<std::vector<PlaybackProgram::to_fpga_message_type> const>>
	    fpga_messages;
	for (auto const& program : programs) {
		if (!program) {
			throw std::runtime_error("run() of invalid shared_ptr<PlaybackProgram> not possible.");
		}
		fpga_messages.push_back(program->get_to_fpga_messages());
	}

	auto result = hxcomm::execute_messages(connection, fpga_messages);

	std::vector<typename decltype(result)::value_type::second_type> connection_time_info;

	for (auto const& single_result : result) {
		connection_time_info.push_back(single_result.second);
	}

	hate::Timer timer;
	auto unique_ids = hxcomm::visit_connection(
	    [](auto& conn) { return conn.get_unique_identifier(); }, connection);
	size_t fpgas_with_fails = 0;
	bool all_tickets_valid = true;
	for (size_t i = 0; i < programs.size(); i++) {
		auto& program = programs.at(i);
		program->clear_from_fpga_messages();

		if constexpr (hate::is_container_v<decltype(result)>) {
			for (auto const& response : result.at(i).first) {
				program->push_from_fpga_message(response);
			}
		} else {
			while (!result.empty()) {
				program->push_from_fpga_message(result.at(i).first);
				result.at(i).pop();
			}
		}
		if (!program->run_ok(unique_ids.at(i))) {
			fpgas_with_fails += 1;
		}
		if (!program->tickets_valid()) {
			all_tickets_valid = false;
		}
	}
	if (fpgas_with_fails > 0) {
		std::stringstream ss;
		ss << fpgas_with_fails;
		if (fpgas_with_fails == 1) {
			ss << " FPGA ";
		} else {
			ss << " FGPAs ";
		}
		ss << "sent error notification(s). Please check for previous error messages.";
		throw std::runtime_error(ss.str());
	}
	if (!all_tickets_valid) {
		throw std::runtime_error("Not all response data to PlaybackPrograms valid.");
	}

	return {connection_time_info, std::chrono::nanoseconds(timer.get_ns())};
}

} // namespace fisch::vx
