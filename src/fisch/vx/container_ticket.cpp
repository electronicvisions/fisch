#include "fisch/vx/container_ticket.h"

#include "fisch/vx/container.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/traits.h"
#include "hate/type_list.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

template <class ContainerT>
ContainerTicket<ContainerT>::ContainerTicket(
    size_t container_count, size_t pos, std::shared_ptr<fisch::vx::PlaybackProgram const> pbp) :
    m_container_count(container_count),
    m_pos(pos),
    m_pbp(pbp)
{
	m_pbp->register_ticket(this);
}

template <class ContainerT>
ContainerTicket<ContainerT>::ContainerTicket(ContainerTicket const& other) :
    m_container_count(other.m_container_count), m_pos(other.m_pos), m_pbp(other.m_pbp)
{
	m_pbp->register_ticket(this);
}

template <class ContainerT>
ContainerTicket<ContainerT>::~ContainerTicket()
{
	m_pbp->deregister_ticket(this);
}

template <class ContainerT>
std::vector<ContainerT> ContainerTicket<ContainerT>::get() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}

		std::vector<ContainerT> containers;

		for (size_t c = 0; c < m_container_count; ++c) {
			auto const& queue =
			    std::get<detail::decode_message_types_index<ContainerT>>(m_pbp->m_receive_queue);

			ContainerT config;
			auto const begin_it =
			    queue.get_messages().cbegin() + m_pos + c * ContainerT::decode_ut_message_count;
			config.decode({begin_it, begin_it + ContainerT::decode_ut_message_count});
			containers.push_back(config);
		}
		return containers;
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
bool ContainerTicket<ContainerT>::valid() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		auto const& queue =
		    std::get<detail::decode_message_types_index<ContainerT>>(m_pbp->m_receive_queue);
		return (
		    queue.size() >= (m_pos + (m_container_count * ContainerT::decode_ut_message_count)));
	} else {
		throw std::logic_error("Container not readable.");
	}
}

template <class ContainerT>
FPGATime ContainerTicket<ContainerT>::fpga_time() const
{
	if constexpr (IsReadable<ContainerT>::value) {
		if (!valid()) {
			throw std::runtime_error("Data not available.");
		}
		auto const& queue =
		    std::get<detail::decode_message_types_index<ContainerT>>(m_pbp->m_receive_queue);
		return *(
		    queue.get_times().cbegin() + m_pos +
		    m_container_count * ContainerT::decode_ut_message_count - 1);
	} else {
		throw std::logic_error("Container not readable.");
	}
}

// explicit instantiation
#define PLAYBACK_CONTAINER(Name, _Type) template class ContainerTicket<Name>;
#include "fisch/vx/container.def"

} // namespace fisch::vx
