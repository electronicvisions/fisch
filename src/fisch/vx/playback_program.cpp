#include "fisch/vx/playback_program.h"

#include "fisch/cerealization.h"
#include "fisch/common/logger.h"
#include "fisch/vx/container.h"
#include "fisch/vx/container_ticket.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/common/cerealization_typed_array.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/common/cerealization_utmessage.h"
#include "hxcomm/vx/utmessage.h"
#include <algorithm>
#include <sstream>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

namespace cereal {

template <typename Archive, typename MessageT>
void CEREAL_SAVE_FUNCTION_NAME(
    Archive& ar,
    fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT> const& queue,
    std::uint32_t const)
{
	auto const& messages = queue.get_messages();
	auto const& times = queue.get_times();
	ar(CEREAL_NVP(messages));
	ar(CEREAL_NVP(times));
}

template <typename Archive, typename MessageT>
void CEREAL_LOAD_FUNCTION_NAME(
    Archive& ar,
    fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT>& queue,
    std::uint32_t const)
{
	std::vector<MessageT> messages;
	std::vector<fisch::vx::FPGATime> times;
	ar(CEREAL_NVP(messages));
	ar(CEREAL_NVP(times));
	queue = fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT>(messages, times);
}

} // namespace cereal

namespace fisch::vx {

PlaybackProgram::PlaybackProgram() :
    m_tickets(),
    m_tickets_mutex(),
    m_instructions(),
    m_receive_queue(),
    m_spike_response_queue(),
    m_madc_sample_response_queue(),
    m_highspeed_link_notification_response_queue(),
    m_spike_pack_counts(),
    m_madc_sample_pack_counts(),
    m_timeout_notification_response_queue(),
    m_decoder(
        m_receive_queue,
        m_spike_response_queue,
        m_madc_sample_response_queue,
        m_highspeed_link_notification_response_queue,
        m_spike_pack_counts,
        m_madc_sample_pack_counts,
        m_timeout_notification_response_queue),
    m_queue_expected_size()
{
	m_queue_expected_size.fill(0);
}

PlaybackProgram::~PlaybackProgram()
{}

template <typename U>
void PlaybackProgram::register_ticket(U* const ticket) const
{
	std::lock_guard<std::mutex> lock(m_tickets_mutex);
	assert((m_tickets.find(ticket) == m_tickets.cend()) && "ticket can't be registered twice.");
	m_tickets.insert(ticket);
}

template <typename U>
void PlaybackProgram::deregister_ticket(U* const ticket) const
{
	std::lock_guard<std::mutex> lock(m_tickets_mutex);
	auto const it = m_tickets.find(ticket);
	assert((it != m_tickets.cend()) && "unknown ticket can't be deregistered.");
	m_tickets.erase(it);
}

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	template void PlaybackProgram::register_ticket(ContainerTicket<Type>* ticket) const;           \
	template void PlaybackProgram::deregister_ticket(ContainerTicket<Type>* ticket) const;
#include "fisch/vx/container.def"

PlaybackProgram::spike_pack_counts_type const& PlaybackProgram::get_spikes_pack_counts() const
{
	return m_spike_pack_counts;
}

PlaybackProgram::madc_sample_pack_counts_type const& PlaybackProgram::get_madc_samples_pack_counts()
    const
{
	return m_madc_sample_pack_counts;
}

PlaybackProgram::spike_from_chip_events_type const& PlaybackProgram::get_spikes() const
{
	return m_spike_response_queue;
}

PlaybackProgram::madc_sample_from_chip_events_type const& PlaybackProgram::get_madc_samples() const
{
	return m_madc_sample_response_queue;
}

PlaybackProgram::highspeed_link_notifications_type const&
PlaybackProgram::get_highspeed_link_notifications() const
{
	return m_highspeed_link_notification_response_queue;
}

bool PlaybackProgram::run_ok() const
{
	// Sophisticated logic to check if we encountered an error.
	auto logger = log4cxx::Logger::getLogger("fisch.PlaybackProgram.run_ok()");
	if (!m_timeout_notification_response_queue.empty()) {
		// reporting first fatal error as we do not expect more than one
		if (m_timeout_notification_response_queue.size() > 1) {
			FISCH_LOG_ERROR(
			    logger, "Program execution illegally triggered multiple error conditions.")
		}
		auto const error = m_timeout_notification_response_queue.front();
		auto const num_context = 10;
		auto const min_instruction = std::max(0u, error.get_value() - num_context);
		std::stringstream ss;
		ss << "Program execution triggered a Playback instruction timeout notification at "
		   << error.get_fpga_time() << ", instruction(" << error.get_value().value()
		   << "; total: " << m_instructions.size() << "). Printing additional context:"
		   << "\n";
		for (auto i = min_instruction; i <= error.get_value().value(); i++) {
			ss << "Instruction(" << i << "): ";
			std::visit([&ss](auto const& ins) { ss << ins; }, m_instructions.at(i));
			ss << "\n";
		}
		FISCH_LOG_ERROR(logger, ss.str());
		return false;
	}
	return true;
}

bool PlaybackProgram::tickets_valid() const
{
	size_t i = 0;
	bool valid = true;
	boost::hana::for_each(m_receive_queue, [this, &valid, &i](auto const& q) {
		valid = valid && q.size() == m_queue_expected_size.at(i);
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
	return m_instructions;
}

void PlaybackProgram::push_from_fpga_message(from_fpga_message_type const& message)
{
	m_decoder(message);
}

void PlaybackProgram::clear_from_fpga_messages()
{
	m_decoder.clear();
}

bool PlaybackProgram::empty() const
{
	return m_instructions.empty();
}

bool PlaybackProgram::operator==(PlaybackProgram const& other) const
{
	return m_instructions == other.m_instructions && m_receive_queue == other.m_receive_queue &&
	       m_spike_response_queue == other.m_spike_response_queue &&
	       m_madc_sample_response_queue == other.m_madc_sample_response_queue &&
	       m_highspeed_link_notification_response_queue ==
	           other.m_highspeed_link_notification_response_queue &&
	       m_spike_pack_counts == other.m_spike_pack_counts &&
	       m_madc_sample_pack_counts == other.m_madc_sample_pack_counts &&
	       m_timeout_notification_response_queue == other.m_timeout_notification_response_queue &&
	       m_queue_expected_size == other.m_queue_expected_size;
}

bool PlaybackProgram::operator!=(PlaybackProgram const& other) const
{
	return !(*this == other);
}

template <typename Archive>
void PlaybackProgram::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_instructions));
	boost::hana::for_each(m_receive_queue, [&ar](auto& queue) { ar(CEREAL_NVP(queue)); });
	ar(CEREAL_NVP(m_spike_response_queue));
	ar(CEREAL_NVP(m_madc_sample_response_queue));
	ar(CEREAL_NVP(m_highspeed_link_notification_response_queue));
	ar(CEREAL_NVP(m_spike_pack_counts));
	ar(CEREAL_NVP(m_madc_sample_pack_counts));
	ar(CEREAL_NVP(m_queue_expected_size));
	ar(CEREAL_NVP(m_timeout_notification_response_queue));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PlaybackProgram)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::PlaybackProgram, 2)
