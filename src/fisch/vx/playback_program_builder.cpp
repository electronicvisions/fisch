#include "fisch/vx/playback_program_builder.h"

#include "fisch/vx/container.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/traits.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hate/type_list.h"
#include "hate/type_traits.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

namespace {

typedef hate::
    type_list<JTAGClockScaler, JTAGIdCode, JTAGPLLRegister, OmnibusChipOverJTAG, ResetJTAGTap>
        jtag_queue_container_list;

typedef hate::
    type_list<OmnibusChip, OmnibusFPGA, SPIShiftRegister, SPIDACDataRegister, SPIDACControlRegister>
        omnibus_queue_container_list;

} // namespace

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
ContainerTicket<typename detail::coordinate_type_to_container_type<CoordinateT>::type>
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
		return ContainerTicket<ContainerT>(coords.size(), pos, m_program);
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
		*ticket_ptr = ContainerTicket<container_type>(
		    ticket_ptr->m_container_count, ticket_ptr->m_pos + translation, m_program);
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

void PlaybackProgramBuilder::copy_back(PlaybackProgramBuilder const& other)
{
	if (!other.is_write_only()) {
		throw std::runtime_error("PlaybackProgramBuilder to copy is not write only.");
	}
	// copy instructions
	std::copy(
	    other.m_program->m_instructions.cbegin(), other.m_program->m_instructions.cend(),
	    std::back_inserter(m_program->m_instructions));
}

bool PlaybackProgramBuilder::empty() const
{
	return m_program->empty();
}

bool PlaybackProgramBuilder::is_write_only() const
{
	return (m_jtag_receive_queue_size == 0) && (m_omnibus_receive_queue_size == 0);
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
