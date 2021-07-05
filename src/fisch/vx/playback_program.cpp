#include "fisch/vx/playback_program.h"

#include "fisch/cerealization.h"
#include "fisch/common/logger.h"
#include "fisch/vx/detail/playback_program_impl.h"
#include "hxcomm/vx/utmessage.h"
#include <algorithm>
#include <sstream>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>
#include <cereal/types/memory.hpp>

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
		ss << "Program execution triggered a Playback instruction timeout notification at "
		   << error.get_fpga_time() << ", instruction(" << error.get_value().value()
		   << "; total: " << m_impl->m_instructions.size() << "). Printing additional context:"
		   << "\n";
		for (auto i = min_instruction; i <= error_value; i++) {
			ss << "Instruction(" << i << "): ";
			std::visit([&ss](auto const& ins) { ss << ins; }, m_impl->m_instructions.at(i));
			ss << "\n";
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

template <typename Archive>
void PlaybackProgram::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_impl));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PlaybackProgram)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::PlaybackProgram, 3)
