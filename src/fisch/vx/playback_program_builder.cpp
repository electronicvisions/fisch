#include "fisch/vx/playback_program_builder.h"

#include "fisch/common/logger.h"
#include "fisch/vx/constants.h"
#include "fisch/vx/container.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/traits.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hate/join.h"
#include "hate/type_list.h"
#include "hate/type_traits.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

PlaybackProgramBuilder::PlaybackProgramBuilder() : m_program(std::make_shared<PlaybackProgram>()) {}

template <class ContainerT>
void PlaybackProgramBuilder::write(
    [[maybe_unused]] typename ContainerT::coordinate_type const& coord, ContainerT const& config)
{
	if constexpr (!IsWritable<ContainerT>::value) {
		std::stringstream ss;
		ss << config << " can't be written.";
		throw std::logic_error(ss.str());
	} else {
		auto const messages = config.encode_write(coord);
		m_program->m_instructions.insert(
		    m_program->m_instructions.end(), messages.begin(), messages.end());
	}
}

template <class ContainerT>
void PlaybackProgramBuilder::write(
    std::vector<typename ContainerT::coordinate_type> const& coords,
    std::vector<ContainerT> const& configs)
{
	size_t const size = coords.size();
	if (configs.size() != size) {
		throw std::runtime_error("Coordinate count does not match configuration count.");
	}

	if constexpr (!IsWritable<ContainerT>::value) {
		std::stringstream ss;
		ss << "Containers [" << hate::join_string(configs, ", ") << "] can't be written.";
		throw std::logic_error(ss.str());
	} else {
		for (size_t i = 0; i < size; ++i) {
			auto const messages = configs[i].encode_write(coords[i]);
			m_program->m_instructions.insert(
			    m_program->m_instructions.end(), messages.cbegin(), messages.cend());
		}
	}
}

template <class CoordinateT>
ContainerTicket<typename detail::coordinate_type_to_container_type<CoordinateT>::type>
PlaybackProgramBuilder::read(CoordinateT const& coord)
{
	typedef typename detail::coordinate_type_to_container_type<CoordinateT>::type ContainerT;

	if constexpr (!IsReadable<ContainerT>::value) {
		std::stringstream ss;

		ss << "Coordinate " << coord << " can't be read.";
		throw std::logic_error(ss.str());
	} else {
		auto const messages = ContainerT::encode_read(coord);

		m_program->m_instructions.insert(
		    m_program->m_instructions.end(), messages.begin(), messages.end());

		size_t& queue_expected_size =
		    m_program->m_queue_expected_size.at(detail::decode_message_types_index<ContainerT>);
		size_t pos = queue_expected_size;
		queue_expected_size += ContainerT::decode_ut_message_count;
		return ContainerTicket<ContainerT>(1, pos, m_program);
	}
}

template <class CoordinateT>
ContainerTicket<typename detail::coordinate_type_to_container_type<CoordinateT>::type>
PlaybackProgramBuilder::read(std::vector<CoordinateT> const& coords)
{
	typedef typename detail::coordinate_type_to_container_type<CoordinateT>::type ContainerT;

	if constexpr (!IsReadable<ContainerT>::value) {
		std::stringstream ss;
		ss << "Coordinates [" << hate::join_string(coords, ", ") << "] can't be read.";
		throw std::logic_error(ss.str());
	} else {
		for (auto const coord : coords) {
			auto const messages = ContainerT::encode_read(coord);
			m_program->m_instructions.insert(
			    m_program->m_instructions.end(), messages.cbegin(), messages.cend());
		}

		size_t& queue_expected_size =
		    m_program->m_queue_expected_size.at(detail::decode_message_types_index<ContainerT>);
		size_t pos = queue_expected_size;
		queue_expected_size += coords.size() * ContainerT::decode_ut_message_count;
		return ContainerTicket<ContainerT>(coords.size(), pos, m_program);
	}
}

std::shared_ptr<PlaybackProgram> PlaybackProgramBuilder::done()
{
	if (size_to_fpga() > playback_memory_size_to_fpga) {
		auto logger = log4cxx::Logger::getLogger("fisch.PlaybackProgramBuilder");
		FISCH_LOG_WARN(
		    logger, "PlaybackProgram instruction size ("
		                << size_to_fpga() << ") larger than playback memory size on FPGA ("
		                << playback_memory_size_to_fpga << ") -> no timing guarantees possible.");
	}
	std::shared_ptr<PlaybackProgram> ret(m_program);
	m_program = std::make_shared<PlaybackProgram>();
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
	m_program->m_instructions.insert(
	    m_program->m_instructions.end(), other.m_program->m_instructions.cbegin(),
	    other.m_program->m_instructions.cend());
	other.m_program->m_instructions.clear();

	// change program in tickets of other builder
	// update position in response queues in tickets of other builder
	auto ticket_changer = [this](auto const& ticket_ptr) {
		typedef hate::remove_all_qualifiers_t<decltype(ticket_ptr)> ticket_type;
		typedef typename ticket_type::container_type container_type;
		if constexpr (IsReadable<container_type>::value) {
			// translation of response queue position
			size_t translation = m_program->m_queue_expected_size.at(
			    detail::decode_message_types_index<container_type>);
			ticket_ptr->m_pbp = m_program;
			ticket_ptr->m_pos += translation;
		}
	};
	for (auto const& ticket_ptr_variant : other.m_program->m_tickets) {
		std::visit(ticket_changer, ticket_ptr_variant);
	}

	// move ticket list from other builder
	m_program->m_tickets.merge(other.m_program->m_tickets);
	assert(other.m_program->m_tickets.empty() && "Ticket(s) are not unique to a PlaybackProgram.");

	// update receive queue sizes
	for (size_t i = 0; i < m_program->m_queue_expected_size.size(); ++i) {
		m_program->m_queue_expected_size.at(i) += other.m_program->m_queue_expected_size.at(i);
	}

	// reset other builder
	other.done();
}

void PlaybackProgramBuilder::merge_front(PlaybackProgramBuilder& other)
{
	// move instructions
	m_program->m_instructions.insert(
	    m_program->m_instructions.begin(), other.m_program->m_instructions.cbegin(),
	    other.m_program->m_instructions.cend());
	other.m_program->m_instructions.clear();

	// change program in tickets of other builder
	auto other_ticket_changer = [this](auto const& ticket_ptr) { ticket_ptr->m_pbp = m_program; };
	for (auto const& ticket_ptr_variant : other.m_program->m_tickets) {
		std::visit(other_ticket_changer, ticket_ptr_variant);
	}

	// update position in response queues in tickets of this builder
	auto ticket_changer = [this, other](auto const& ticket_ptr) {
		typedef hate::remove_all_qualifiers_t<decltype(ticket_ptr)> ticket_type;
		typedef typename ticket_type::container_type container_type;
		if constexpr (IsReadable<container_type>::value) {
			// translation of response queue position
			size_t translation = m_program->m_queue_expected_size.at(
			    detail::decode_message_types_index<container_type>);
			ticket_ptr->m_pos += translation;
		}
	};
	for (auto const& ticket_ptr_variant : m_program->m_tickets) {
		std::visit(ticket_changer, ticket_ptr_variant);
	}

	// move ticket list from other builder
	m_program->m_tickets.merge(other.m_program->m_tickets);
	assert(other.m_program->m_tickets.empty() && "Ticket(s) are not unique to a PlaybackProgram.");

	// update receive queue sizes
	for (size_t i = 0; i < m_program->m_queue_expected_size.size(); ++i) {
		m_program->m_queue_expected_size.at(i) += other.m_program->m_queue_expected_size.at(i);
	}

	// reset other builder
	other.done();
}

void PlaybackProgramBuilder::copy_back(PlaybackProgramBuilder const& other)
{
	if (!other.is_write_only()) {
		throw std::runtime_error("PlaybackProgramBuilder to copy is not write only.");
	}
	// copy instructions
	m_program->m_instructions.insert(
	    m_program->m_instructions.end(), other.m_program->m_instructions.cbegin(),
	    other.m_program->m_instructions.cend());
}

bool PlaybackProgramBuilder::empty() const
{
	return m_program->empty();
}

size_t PlaybackProgramBuilder::size_to_fpga() const
{
	return m_program->m_instructions.size();
}

bool PlaybackProgramBuilder::is_write_only() const
{
	return std::all_of(
	    m_program->m_queue_expected_size.begin(), m_program->m_queue_expected_size.end(),
	    [](auto const s) { return s == 0; });
}

// explicit instantiation
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	template ContainerTicket<Name> PlaybackProgramBuilder::read<typename Name::coordinate_type>(   \
	    typename Name::coordinate_type const& coord);                                              \
	template ContainerTicket<Name> PlaybackProgramBuilder::read<typename Name::coordinate_type>(   \
	    std::vector<typename Name::coordinate_type> const& coord);                                 \
	template void PlaybackProgramBuilder::write<Name>(                                             \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	template void PlaybackProgramBuilder::write<Name>(                                             \
	    std::vector<typename Name::coordinate_type> const& coords,                                 \
	    std::vector<Name> const& configs);
#include "fisch/vx/container.def"

} // namespace fisch::vx
