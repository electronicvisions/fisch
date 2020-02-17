#pragma once

#include "fisch/vx/playback_program.h"

#include "hxcomm/vx/instruction/system.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx::detail {

/**
 * Run the given FPGA messages on a locally connected Connection and return the results.
 *
 * @param conn Connection on which to transmit the UT messages and await results.
 * @param messages UT messages to send.
 * @return Response from the local connection.
 */
template <typename Connection>
std::vector<PlaybackProgram::from_fpga_message_type> execute_words(
    Connection& conn, std::vector<PlaybackProgram::to_fpga_message_type> const& messages)
{
	conn.add(messages);
	conn.add(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Halt>());
	conn.commit();

	conn.run_until_halt();

	std::vector<PlaybackProgram::from_fpga_message_type> responses;
	typename PlaybackProgram::from_fpga_message_type response;
	while (conn.try_receive(response)) {
		responses.push_back(response);
	}
	return responses;
}

} // namespace fisch::vx::detail
