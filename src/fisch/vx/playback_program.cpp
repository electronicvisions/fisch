#include "fisch/vx/playback_program.h"

#include "fisch/vx/container.h"
#include "fisch/vx/traits.h"

#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>

#include "fisch/cerealization.h"
#include "fisch/vx/container.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/common/cerealization_typed_array.h"
#include "hate/type_list.h"
#include "hate/type_traits.h"
#include "hxcomm/vx/utmessage.h"
#include "hxcomm/common/cerealization_utmessage.h"

namespace cereal {

template <typename Archive, typename MessageT>
void CEREAL_SAVE_FUNCTION_NAME(
    Archive& ar, fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT> const& queue)
{
	auto const& messages = queue.get_messages();
	auto const& times = queue.get_times();
	ar(CEREAL_NVP(messages));
	ar(CEREAL_NVP(times));
}

template <typename Archive, typename MessageT>
void CEREAL_LOAD_FUNCTION_NAME(
    Archive& ar, fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT>& queue)
{
	std::vector<MessageT> messages;
	std::vector<fisch::vx::FPGATime> times;
	ar(CEREAL_NVP(messages));
	ar(CEREAL_NVP(times));
	queue = fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT>(messages, times);
}

} // namespace cereal

namespace fisch::vx {

namespace {

typedef hate::
    type_list<JTAGClockScaler, JTAGIdCode, JTAGPLLRegister, OmnibusChipOverJTAG, ResetJTAGTap>
        jtag_queue_container_list;

typedef hate::
    type_list<OmnibusChip, OmnibusFPGA, SPIShiftRegister, SPIDACDataRegister, SPIDACControlRegister>
        omnibus_queue_container_list;

} // namespace

template <class ContainerT>
PlaybackProgram::ContainerTicket<ContainerT>::ContainerTicket(
    size_t pos, std::shared_ptr<PlaybackProgram const> pbp) :
    m_pos(pos),
    m_pbp(pbp)
{
	m_pbp->register_ticket(this);
}

template <class ContainerT>
PlaybackProgram::ContainerTicket<ContainerT>::ContainerTicket(ContainerTicket const& other) :
    m_pos(other.m_pos),
    m_pbp(other.m_pbp)
{
	m_pbp->register_ticket(this);
}

template <class ContainerT>
PlaybackProgram::ContainerTicket<ContainerT>::~ContainerTicket()
{
	m_pbp->deregister_ticket(this);
}

template <class ContainerT>
ContainerT PlaybackProgram::ContainerTicket<ContainerT>::get() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}
		ContainerT config;
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			auto const begin_it = m_pbp->m_receive_queue_omnibus.get_messages().cbegin() + m_pos;
			config.decode({begin_it, begin_it + ContainerT::decode_ut_message_count});
		} else if constexpr (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			auto const begin_it = m_pbp->m_receive_queue_jtag.get_messages().cbegin() + m_pos;
			config.decode({begin_it, begin_it + ContainerT::decode_ut_message_count});
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
		return config;
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
bool PlaybackProgram::ContainerTicket<ContainerT>::valid() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		size_t queue_size = 0;
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			queue_size = m_pbp->m_receive_queue_omnibus.size();
		} else if constexpr (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			queue_size = m_pbp->m_receive_queue_jtag.size();
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
		return (queue_size >= (m_pos + ContainerT::decode_ut_message_count));
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
FPGATime PlaybackProgram::ContainerTicket<ContainerT>::fpga_time() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			return *(
			    m_pbp->m_receive_queue_omnibus.get_times().cbegin() + m_pos +
			    ContainerT::decode_ut_message_count - 1);
		} else if constexpr (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			return *(
			    m_pbp->m_receive_queue_jtag.get_times().cbegin() + m_pos +
			    ContainerT::decode_ut_message_count - 1);
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
PlaybackProgram::ContainerVectorTicket<ContainerT>::ContainerVectorTicket(
    size_t container_count, size_t pos, std::shared_ptr<fisch::vx::PlaybackProgram const> pbp) :
    m_container_count(container_count),
    m_pos(pos),
    m_pbp(pbp)
{
	m_pbp->register_ticket(this);
}

template <class ContainerT>
PlaybackProgram::ContainerVectorTicket<ContainerT>::ContainerVectorTicket(
    ContainerVectorTicket const& other) :
    m_container_count(other.m_container_count),
    m_pos(other.m_pos),
    m_pbp(other.m_pbp)
{
	m_pbp->register_ticket(this);
}

template <class ContainerT>
PlaybackProgram::ContainerVectorTicket<ContainerT>::~ContainerVectorTicket()
{
	m_pbp->deregister_ticket(this);
}

template <class ContainerT>
std::vector<ContainerT> PlaybackProgram::ContainerVectorTicket<ContainerT>::get() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}

		std::vector<ContainerT> containers;

		for (size_t c = 0; c < m_container_count; ++c) {
			ContainerT config;
			if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
				auto const begin_it = m_pbp->m_receive_queue_omnibus.get_messages().cbegin() +
				                      m_pos + c * ContainerT::decode_ut_message_count;
				config.decode({begin_it, begin_it + ContainerT::decode_ut_message_count});
			} else if constexpr (hate::is_in_type_list<
			                         ContainerT, jtag_queue_container_list>::value) {
				auto const begin_it = m_pbp->m_receive_queue_jtag.get_messages().cbegin() + m_pos +
				                      c * ContainerT::decode_ut_message_count;
				config.decode({begin_it, begin_it + ContainerT::decode_ut_message_count});
			} else {
				throw std::logic_error("Container response queue not implemented.");
			}
			containers.push_back(config);
		}
		return containers;
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
bool PlaybackProgram::ContainerVectorTicket<ContainerT>::valid() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		size_t queue_size;
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			queue_size = m_pbp->m_receive_queue_omnibus.size();
		} else if constexpr (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			queue_size = m_pbp->m_receive_queue_jtag.size();
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
		return (queue_size >= (m_pos + (m_container_count * ContainerT::decode_ut_message_count)));
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
FPGATime PlaybackProgram::ContainerVectorTicket<ContainerT>::fpga_time() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			return *(
			    m_pbp->m_receive_queue_omnibus.get_times().cbegin() + m_pos +
			    m_container_count * ContainerT::decode_ut_message_count - 1);
		} else if (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			return *(
			    m_pbp->m_receive_queue_jtag.get_times().cbegin() + m_pos +
			    m_container_count * ContainerT::decode_ut_message_count - 1);
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
	} else {
		throw std::logic_error("Container not readable.");
	}
}

PlaybackProgram::PlaybackProgram() :
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

template <typename U>
void PlaybackProgram::register_ticket(U* const ticket) const
{
	assert((m_tickets.find(ticket) == m_tickets.cend()) && "ticket can't be registered twice.");
	m_tickets.insert(ticket);
}

template <typename U>
void PlaybackProgram::deregister_ticket(U* const ticket) const
{
	auto const it = m_tickets.find(ticket);
	assert((it != m_tickets.cend()) && "unknown ticket can't be deregistered.");
	m_tickets.erase(it);
}

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
void PlaybackProgram::serialize(Archive& ar)
{
	ar(m_instructions);
	ar(m_receive_queue_jtag);
	ar(m_receive_queue_omnibus);
	ar(m_spike_response_queue);
	ar(m_madc_sample_response_queue);
	ar(m_spike_pack_counts);
	ar(m_madc_sample_pack_counts);
	ar(m_jtag_queue_expected_size);
	ar(m_omnibus_queue_expected_size);
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PlaybackProgram)

PlaybackProgramBuilder::PlaybackProgramBuilder() :
    m_program(std::make_shared<PlaybackProgram>()),
    m_jtag_receive_queue_size(0),
    m_omnibus_receive_queue_size(0)
{}

// TODO: to be modified once multi-timer support available
void PlaybackProgramBuilder::wait_until(
    Timer::coordinate_type const& /* coord */, Timer::Value time)
{
	m_program->m_instructions.push_back(
	    hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::WaitUntil>(time.value()));
}

template <class ContainerT>
void PlaybackProgramBuilder::write(
    typename ContainerT::coordinate_type const& coord, ContainerT const& config)
{
	if constexpr (!IsWritable<ContainerT>::value) {
		std::stringstream ss;
		ss << config << " can't be written.";
		throw std::logic_error(ss.str());
	} else {
		auto messages = config.encode_write(coord);
		m_program->m_instructions.insert(
		    m_program->m_instructions.end(), messages.begin(), messages.end());
	}
}

template <class ContainerT>
void PlaybackProgramBuilder::write(
    std::vector<typename ContainerT::coordinate_type> const& coords,
    std::vector<ContainerT> const& configs)
{
	if (coords.size() != configs.size()) {
		throw std::runtime_error("Coordinate count does not match configuration count.");
	}

	for (size_t i = 0; i < coords.size(); ++i) {
		write(coords[i], configs[i]);
	}
}

template <class CoordinateT>
PlaybackProgram::ContainerTicket<
    typename detail::coordinate_type_to_container_type<CoordinateT>::type>
PlaybackProgramBuilder::read(CoordinateT const& coord)
{
	typedef typename detail::coordinate_type_to_container_type<CoordinateT>::type ContainerT;

	if constexpr (!IsReadable<ContainerT>::value) {
		std::stringstream ss;

		ss << "Coordinate " << coord << " can't be read.";
		throw std::logic_error(ss.str());
	} else {
		auto messages = ContainerT::encode_read(coord);

		m_program->m_instructions.insert(
		    m_program->m_instructions.end(), messages.begin(), messages.end());

		size_t pos;
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			pos = m_omnibus_receive_queue_size;
			m_omnibus_receive_queue_size += ContainerT::decode_ut_message_count;
		} else if constexpr (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			pos = m_jtag_receive_queue_size;
			m_jtag_receive_queue_size += ContainerT::decode_ut_message_count;
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
		return PlaybackProgram::ContainerTicket<ContainerT>(pos, m_program);
	}
}

template <class CoordinateT>
PlaybackProgram::ContainerVectorTicket<
    typename detail::coordinate_type_to_container_type<CoordinateT>::type>
PlaybackProgramBuilder::read(std::vector<CoordinateT> const& coords)
{
	typedef typename detail::coordinate_type_to_container_type<CoordinateT>::type ContainerT;

	if constexpr (!IsReadable<ContainerT>::value) {
		std::stringstream ss;
		ss << "Coordinates [";
		for (auto it = coords.cbegin(); it != coords.cend(); ++it) {
			ss << *it;
			if (it != (coords.cend() - 1)) {
				ss << ", ";
			}
		}
		ss << "] can't be read.";
		throw std::logic_error(ss.str());
	} else {
		for (auto coord : coords) {
			auto messages = ContainerT::encode_read(coord);

			m_program->m_instructions.insert(
			    m_program->m_instructions.end(), messages.begin(), messages.end());
		}

		size_t pos;
		if constexpr (hate::is_in_type_list<ContainerT, omnibus_queue_container_list>::value) {
			pos = m_omnibus_receive_queue_size;
			m_omnibus_receive_queue_size += coords.size() * ContainerT::decode_ut_message_count;
		} else if constexpr (hate::is_in_type_list<ContainerT, jtag_queue_container_list>::value) {
			pos = m_jtag_receive_queue_size;
			m_jtag_receive_queue_size += coords.size() * ContainerT::decode_ut_message_count;
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
		return PlaybackProgram::ContainerVectorTicket<ContainerT>(coords.size(), pos, m_program);
	}
}

std::shared_ptr<PlaybackProgram> PlaybackProgramBuilder::done()
{
	m_program->m_jtag_queue_expected_size = m_jtag_receive_queue_size;
	m_program->m_omnibus_queue_expected_size = m_omnibus_receive_queue_size;
	std::shared_ptr<PlaybackProgram> ret(m_program);
	m_program = std::make_shared<PlaybackProgram>();
	m_jtag_receive_queue_size = 0;
	m_omnibus_receive_queue_size = 0;
	return ret;
}

std::ostream& operator<<(std::ostream& os, PlaybackProgramBuilder const& builder)
{
	os << *(builder.m_program);
	return os;
}

void PlaybackProgramBuilder::merge_back(PlaybackProgramBuilder& other)
{
	// move instructions
	std::copy(
	    other.m_program->m_instructions.cbegin(), other.m_program->m_instructions.cend(),
	    std::back_inserter(m_program->m_instructions));
	other.m_program->m_instructions.clear();

	// change program in tickets of other builder
	// update position in response queues in tickets of other builder
	auto ticket_changer = [this](auto const& ticket_ptr) {
		typedef hate::remove_all_qualifiers_t<decltype(ticket_ptr)> ticket_type;
		typedef typename ticket_type::container_type container_type;
		size_t translation = 0; // translation of response queue position
		if constexpr (hate::is_in_type_list<container_type, omnibus_queue_container_list>::value) {
			translation = m_omnibus_receive_queue_size;
		} else if constexpr (hate::is_in_type_list<
		                         container_type, jtag_queue_container_list>::value) {
			translation = m_jtag_receive_queue_size;
		} else {
			throw std::logic_error("Container response queue not implemented.");
		}
		if constexpr (std::is_same<
		                  ticket_type, PlaybackProgram::ContainerTicket<container_type>>::value) {
			*ticket_ptr = PlaybackProgram::ContainerTicket<container_type>(
			    ticket_ptr->m_pos + translation, m_program);
		} else {
			*ticket_ptr = PlaybackProgram::ContainerVectorTicket<container_type>(
			    ticket_ptr->m_container_count, ticket_ptr->m_pos + translation, m_program);
		}
	};
	for (auto const& ticket_ptr_variant : other.m_program->m_tickets) {
		std::visit(ticket_changer, ticket_ptr_variant);
	}

	// move ticket list from other builder
	m_program->m_tickets.merge(other.m_program->m_tickets);
	assert(other.m_program->m_tickets.empty() && "Ticket(s) are not unique to a PlaybackProgram.");

	// update receive queue sizes
	m_jtag_receive_queue_size += other.m_jtag_receive_queue_size;
	m_omnibus_receive_queue_size += other.m_omnibus_receive_queue_size;

	// reset other builder
	other.done();
}

bool PlaybackProgramBuilder::empty() const
{
	return m_program->m_instructions.empty();
}

// explicit instantiation
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	template class PlaybackProgram::ContainerTicket<Name>;                                         \
	template class PlaybackProgram::ContainerVectorTicket<Name>;                                   \
	template PlaybackProgram::ContainerTicket<Name>                                                \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    typename Name::coordinate_type const& coord);                                              \
	template PlaybackProgram::ContainerVectorTicket<Name>                                          \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    std::vector<typename Name::coordinate_type> const& coord);                                 \
	template void PlaybackProgramBuilder::write<Name>(                                             \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	template void PlaybackProgramBuilder::write<Name>(                                             \
	    std::vector<typename Name::coordinate_type> const& coords,                                 \
	    std::vector<Name> const& configs);
#include "fisch/vx/container.def"

} // namespace fisch::vx
