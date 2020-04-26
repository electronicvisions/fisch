#include "fisch/vx/container_ticket.h"

#include "fisch/vx/container.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/traits.h"
#include "hate/type_list.h"
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
bool ContainerTicket<ContainerT>::valid() const
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
FPGATime ContainerTicket<ContainerT>::fpga_time() const
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

// explicit instantiation
#define PLAYBACK_CONTAINER(Name, _Type) template class ContainerTicket<Name>;
#include "fisch/vx/container.def"

} // namespace fisch::vx
