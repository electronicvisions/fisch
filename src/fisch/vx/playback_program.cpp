#include "fisch/vx/playback_program.h"

#include "fisch/vx/jtag.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/timer.h"

namespace fisch::vx {

template <class ContainerT>
PlaybackProgram::ContainerTicket<ContainerT>::ContainerTicket(
    size_t jtag_pos, std::shared_ptr<PlaybackProgram const> pbp) :
    jtag_pos(jtag_pos),
    pbp(pbp)
{}

template <class ContainerT>
ContainerT PlaybackProgram::ContainerTicket<ContainerT>::get() const
{
	if (!valid()) {
		throw std::runtime_error("Data not available.");
	}
	std::array<hxcomm::vx::ut_message_from_fpga_variant, ContainerT::decode_ut_message_count>
	    messages;
	// FIXME: unnecessary copy, Issue 3132
	std::copy(
	    pbp->m_receive_queue_jtag.begin() + jtag_pos,
	    pbp->m_receive_queue_jtag.begin() + jtag_pos + ContainerT::decode_ut_message_count,
	    messages.begin());
	ContainerT config;
	config.decode(messages);
	return config;
}

template <class ContainerT>
bool PlaybackProgram::ContainerTicket<ContainerT>::valid() const
{
	return !(pbp->m_receive_queue_jtag.size() < (jtag_pos + ContainerT::decode_ut_message_count));
}

template <class ContainerT>
PlaybackProgram::ContainerVectorTicket<ContainerT>::ContainerVectorTicket(
    size_t container_count,
    size_t jtag_pos,
    std::shared_ptr<fisch::vx::PlaybackProgram const> pbp) :
    container_count(container_count),
    jtag_pos(jtag_pos),
    pbp(pbp)
{}

template <class ContainerT>
std::vector<ContainerT> PlaybackProgram::ContainerVectorTicket<ContainerT>::get() const
{
	if (!valid()) {
		throw std::runtime_error("Data not available.");
	}

	std::vector<ContainerT> containers;

	for (size_t c = 0; c < container_count; ++c) {
		std::array<hxcomm::vx::ut_message_from_fpga_variant, ContainerT::decode_ut_message_count>
		    messages;
		// FIXME: unnecessary copy, Issue 3132
		std::copy(
		    pbp->m_receive_queue_jtag.begin() + jtag_pos + c * ContainerT::decode_ut_message_count,
		    pbp->m_receive_queue_jtag.begin() + jtag_pos +
		        (c + 1) * ContainerT::decode_ut_message_count,
		    messages.begin());
		ContainerT config;
		config.decode(messages);
		containers.push_back(config);
	}
	return containers;
}

template <class ContainerT>
bool PlaybackProgram::ContainerVectorTicket<ContainerT>::valid() const
{
	return !(
	    pbp->m_receive_queue_jtag.size() <
	    (jtag_pos + (container_count * ContainerT::decode_ut_message_count)));
}

std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program)
{
	auto const& messages = program.get_send_messages();
	for (auto it = messages.cbegin(); it < messages.cend(); ++it) {
		if (it != messages.cbegin()) {
			os << std::endl;
		}
		boost::apply_visitor([&os](auto m) { os << m; }, *it);
	}
	return os;
}

std::vector<PlaybackProgram::send_message_type> const& PlaybackProgram::get_send_messages() const
{
	return m_instructions;
}

void PlaybackProgram::push_received_message(receive_message_type const& message)
{
	if (auto message_ptr =
	        boost::get<typename decltype(m_receive_queue_jtag)::value_type>(&message)) {
		m_receive_queue_jtag.push_back(*message_ptr);
	} else if (boost::get<hxcomm::vx::ut_message_from_fpga<
	               hxcomm::vx::instruction::from_fpga_system::halt>>(&message)) {
		// ignore halt response
	} else {
		std::stringstream ss;
		ss << "Response message processing of ";
		boost::apply_visitor([&ss](auto m) { ss << m; }, message);
		ss << " unimplemented.";
		throw std::runtime_error(ss.str());
	}
}


PlaybackProgramBuilder::PlaybackProgramBuilder() :
    m_program(std::make_shared<PlaybackProgram>()),
    m_jtag_receive_queue_size(0)
{}

// TODO: to be modified once multi-timer support available
void PlaybackProgramBuilder::wait_until(
    Timer::coordinate_type const& /* coord */, Timer::Value time)
{
	m_program->m_instructions.push_back(
	    hxcomm::vx::ut_message_to_fpga<hxcomm::vx::instruction::timing::wait_until>(time.value()));
}

template <class ContainerT>
void PlaybackProgramBuilder::write(
    typename ContainerT::coordinate_type const& coord, ContainerT const& config)
{
	auto messages = config.encode_write(coord);
	m_program->m_instructions.insert(
	    m_program->m_instructions.end(), messages.begin(), messages.end());
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
		auto messages = configs[i].encode_write(coords[i]);
		m_program->m_instructions.insert(
		    m_program->m_instructions.end(), messages.begin(), messages.end());
	}
}

template <class ContainerT>
PlaybackProgram::ContainerTicket<ContainerT> PlaybackProgramBuilder::read(
    typename ContainerT::coordinate_type const& coord)
{
	auto messages = ContainerT::encode_read(coord);

	m_program->m_instructions.insert(
	    m_program->m_instructions.end(), messages.begin(), messages.end());

	size_t jtag_pos = m_jtag_receive_queue_size;
	m_jtag_receive_queue_size += ContainerT::decode_ut_message_count;

	return PlaybackProgram::ContainerTicket<ContainerT>(jtag_pos, m_program);
}

template <class ContainerT>
PlaybackProgram::ContainerVectorTicket<ContainerT> PlaybackProgramBuilder::read(
    std::vector<typename ContainerT::coordinate_type> const& coords)
{
	for (auto coord : coords) {
		auto messages = ContainerT::encode_read(coord);

		m_program->m_instructions.insert(
		    m_program->m_instructions.end(), messages.begin(), messages.end());
	}
	size_t jtag_pos = m_jtag_receive_queue_size;
	m_jtag_receive_queue_size += coords.size() * ContainerT::decode_ut_message_count;

	return PlaybackProgram::ContainerVectorTicket<ContainerT>(coords.size(), jtag_pos, m_program);
}

void PlaybackProgramBuilder::halt()
{
	m_program->m_instructions.push_back(
	    hxcomm::vx::ut_message_to_fpga<hxcomm::vx::instruction::system::halt>());
}

std::shared_ptr<PlaybackProgram> PlaybackProgramBuilder::done()
{
	std::shared_ptr<PlaybackProgram> ret(m_program);
	m_program = std::make_shared<PlaybackProgram>();
	m_jtag_receive_queue_size = 0;
	return ret;
}

// explicit instantiation
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	template class PlaybackProgram::ContainerTicket<Name>;                                         \
	template class PlaybackProgram::ContainerVectorTicket<Name>;                                   \
	template PlaybackProgram::ContainerTicket<Name> PlaybackProgramBuilder::read<Name>(            \
	    typename Name::coordinate_type const& coord);                                              \
	template PlaybackProgram::ContainerVectorTicket<Name> PlaybackProgramBuilder::read<Name>(      \
	    std::vector<typename Name::coordinate_type> const& coord);                                 \
	template void PlaybackProgramBuilder::write<Name>(                                             \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	template void PlaybackProgramBuilder::write<Name>(                                             \
	    std::vector<typename Name::coordinate_type> const& coords,                                 \
	    std::vector<Name> const& configs);
#include "fisch/vx/container.def"

} // namespace fisch::vx
