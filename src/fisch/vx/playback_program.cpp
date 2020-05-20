#include "fisch/vx/playback_program.h"

#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>
#include "fisch/cerealization.h"
#include "fisch/vx/container.h"
#include "fisch/vx/container_ticket.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/common/cerealization_typed_array.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/common/cerealization_utmessage.h"
#include "hxcomm/vx/utmessage.h"

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
    m_receive_queue_jtag(),
    m_receive_queue_omnibus(),
    m_spike_response_queue(),
    m_madc_sample_response_queue(),
    m_spike_pack_counts(),
    m_madc_sample_pack_counts(),
    m_decoder(
        m_receive_queue_jtag,
        m_receive_queue_omnibus,
        m_spike_response_queue,
        m_madc_sample_response_queue,
        m_spike_pack_counts,
        m_madc_sample_pack_counts),
    m_jtag_queue_expected_size(0),
    m_omnibus_queue_expected_size(0)
{}

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

bool PlaybackProgram::valid() const
{
	return m_receive_queue_jtag.size() == m_jtag_queue_expected_size &&
	       m_receive_queue_omnibus.size() == m_omnibus_queue_expected_size;
}

std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program)
{
	auto const& messages = program.get_to_fpga_messages();
	for (auto it = messages.cbegin(); it < messages.cend(); ++it) {
		if (it != messages.cbegin()) {
			os << std::endl;
		}
		boost::apply_visitor([&os](auto m) { os << m; }, *it);
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
	return m_instructions == other.m_instructions &&
	       m_receive_queue_jtag == other.m_receive_queue_jtag &&
	       m_receive_queue_omnibus == other.m_receive_queue_omnibus &&
	       m_spike_response_queue == other.m_spike_response_queue &&
	       m_madc_sample_response_queue == other.m_madc_sample_response_queue &&
	       m_spike_pack_counts == other.m_spike_pack_counts &&
	       m_madc_sample_pack_counts == other.m_madc_sample_pack_counts &&
	       m_jtag_queue_expected_size == other.m_jtag_queue_expected_size &&
	       m_omnibus_queue_expected_size == other.m_omnibus_queue_expected_size;
}

bool PlaybackProgram::operator!=(PlaybackProgram const& other) const
{
	return !(*this == other);
}

template <typename Archive>
void PlaybackProgram::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_instructions));
	ar(CEREAL_NVP(m_receive_queue_jtag));
	ar(CEREAL_NVP(m_receive_queue_omnibus));
	ar(CEREAL_NVP(m_spike_response_queue));
	ar(CEREAL_NVP(m_madc_sample_response_queue));
	ar(CEREAL_NVP(m_spike_pack_counts));
	ar(CEREAL_NVP(m_madc_sample_pack_counts));
	ar(CEREAL_NVP(m_jtag_queue_expected_size));
	ar(CEREAL_NVP(m_omnibus_queue_expected_size));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PlaybackProgram)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::PlaybackProgram, 0)
