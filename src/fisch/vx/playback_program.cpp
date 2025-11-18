#include "fisch/vx/playback_program.h"

#include "fisch/common/logger.h"
#include "fisch/vx/detail/playback_program_impl.h"
#include "hxcomm/vx/utmessage.h"
#include <algorithm>
#include <sstream>
#include <type_traits>
#include <variant>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>

namespace fisch::vx {

PlaybackProgram::PlaybackProgram() : m_impl(std::make_unique<detail::PlaybackProgramImpl>()) {}

PlaybackProgram::~PlaybackProgram()
{}

PlaybackProgram::spike_pack_counts_type const& PlaybackProgram::get_spikes_pack_counts() const
{
	assert(m_impl);
	return m_impl->m_spike_pack_counts;
}

PlaybackProgram::madc_sample_pack_counts_type const& PlaybackProgram::get_madc_samples_pack_counts()
    const
{
	assert(m_impl);
	return m_impl->m_madc_sample_pack_counts;
}

PlaybackProgram::spike_from_chip_events_type const& PlaybackProgram::get_spikes() const
{
	assert(m_impl);
	return m_impl->m_spike_response_queue;
}

PlaybackProgram::madc_sample_from_chip_events_type const& PlaybackProgram::get_madc_samples() const
{
	assert(m_impl);
	return m_impl->m_madc_sample_response_queue;
}

PlaybackProgram::highspeed_link_notifications_type const&
PlaybackProgram::get_highspeed_link_notifications() const
{
	assert(m_impl);
	return m_impl->m_highspeed_link_notification_response_queue;
}

bool PlaybackProgram::run_ok() const
{
	assert(m_impl);
	// Sophisticated logic to check if we encountered an error.
	auto logger = log4cxx::Logger::getLogger("fisch.PlaybackProgram.run_ok()");
	if (!m_impl->m_timeout_notification_response_queue.empty()) {
		// reporting first fatal error as we do not expect more than one
		if (m_impl->m_timeout_notification_response_queue.size() > 1) {
			FISCH_LOG_ERROR(
			    logger, "Program execution illegally triggered multiple error conditions.")
		}
		auto const error = m_impl->m_timeout_notification_response_queue.front();
		auto const error_value = error.get_value().value();
		auto const num_context = 10u;
		auto const min_instruction = error_value - std::min(error_value, num_context);
		std::stringstream ss;
		if (error_value > m_impl->m_instructions.size()) {
			ss << "Program execution triggered a Playback instruction timeout. "
			   << "Index to timed-out instruction provided by hardware is invalid: "
			   << "FPGA reported " << error_value << " but the program size is only "
			   << m_impl->m_instructions.size() << "\n";
			FISCH_LOG_ERROR(logger, ss.str());
			return false;
		}
		ss << "Program execution triggered a Playback instruction timeout notification:\n"
		   << "  fpga_time    : " << error.get_fpga_time() << ",\n"
		   << "  instruction  : " << error_value << " of total " << m_impl->m_instructions.size()
		   << ",\n"
		   << "  trace_stalls : " << error.get_trace_stall().value()
		   << " inside sliding window of size " << error.get_trace_stall().max << ",\n"
		   << "  omnibus_reads: " << error.get_omnibus_reads().value()
		   << " in flight of highest tree depth " << error.get_omnibus_reads().max << ".\n"
		   << "Printing additional context:\n";
		for (auto i = min_instruction; i <= error_value; i++) {
			ss << "Instruction(" << i << "): ";
			std::visit([&ss](auto const& ins) { ss << ins; }, m_impl->m_instructions.at(i));
			ss << "\n";
		}
		auto const omnibus_reads_value = error.get_omnibus_reads().value();
		if (omnibus_reads_value > 0) {
			ss << "Printing playback memory from last successful omnibus read to the timed-out "
			      "instruction:\n";
			using NoticountT = std::remove_const<decltype(omnibus_reads_value)>::type;
			NoticountT instruction_last_successful = 0, instruction_first_incomplete = 0;
			for (NoticountT i = error_value, no_reads = 0; i > 0; i--) {
				std::visit(
				    [&no_reads](auto const& ins) {
					    using T = std::decay_t<decltype(ins)>;
					    if constexpr (std::is_same_v<
					                      T,
					                      hxcomm::vx::UTMessageToFPGA<
					                          hxcomm::vx::instruction::omnibus_to_fpga::Address>>) {
						    auto address_payload = ins.decode();
						    if (address_payload.get_is_read()) {
							    no_reads++;
						    }
					    }
				    },
				    m_impl->m_instructions.at(i));
				if (no_reads == omnibus_reads_value) {
					instruction_first_incomplete = i;
				}
				if (no_reads > omnibus_reads_value) {
					instruction_last_successful = i;
					break;
				}
			}
			for (NoticountT i = instruction_last_successful; i <= error_value; i++) {
				if (i == instruction_first_incomplete) {
					ss << "-> ";
				} else {
					ss << "   ";
				}
				ss << "Instruction(" << i << "): ";
				std::visit([&ss](auto const& ins) { ss << ins; }, m_impl->m_instructions.at(i));
				ss << "\n";
			}
		}
		FISCH_LOG_ERROR(logger, ss.str());
		return false;
	}
	return true;
}

bool PlaybackProgram::tickets_valid() const
{
	assert(m_impl);
	size_t i = 0;
	bool valid = true;
	boost::hana::for_each(m_impl->m_receive_queue, [this, &valid, &i](auto const& q) {
		valid = valid && q.size() == m_impl->m_queue_expected_size.at(i);
		i++;
	});
	return valid;
}

std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program)
{
	auto const& messages = program.get_to_fpga_messages();
	for (auto it = messages.cbegin(); it < messages.cend(); ++it) {
		if (it != messages.cbegin()) {
			os << std::endl;
		}
		std::visit([&os](auto m) { os << m; }, *it);
	}
	return os;
}

std::vector<PlaybackProgram::to_fpga_message_type> const& PlaybackProgram::get_to_fpga_messages()
    const
{
	assert(m_impl);
	return m_impl->m_instructions;
}

void PlaybackProgram::push_from_fpga_message(from_fpga_message_type const& message)
{
	assert(m_impl);
	m_impl->m_decoder(message);
}

void PlaybackProgram::clear_from_fpga_messages()
{
	assert(m_impl);
	m_impl->m_decoder.clear();
}

bool PlaybackProgram::empty() const
{
	assert(m_impl);
	return m_impl->m_instructions.empty();
}

bool PlaybackProgram::operator==(PlaybackProgram const& other) const
{
	assert(m_impl);
	assert(other.m_impl);
	return *m_impl == *other.m_impl;
}

bool PlaybackProgram::operator!=(PlaybackProgram const& other) const
{
	return !(*this == other);
}

} // namespace fisch::vx
