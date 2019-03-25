#pragma once

#include "fisch/vx/genpybind.h"
#include "fisch/vx/timer.h"
#include "hxcomm/vx/utmessage.h"

#ifdef __GENPYBIND__
#include "fisch/vx/jtag.h"
#include "fisch/vx/reset.h"
#endif // __GENPYBIND__

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgramBuilder;

/** Playback program. */
class GENPYBIND(visible, holder_type("std::shared_ptr<fisch::vx::PlaybackProgram>")) PlaybackProgram
    : public std::enable_shared_from_this<PlaybackProgram>
{
public:
	/**
	 * Single container read-ticket.
	 * @tparam ContainerT Container type of corresponding data.
	 */
	template <class ContainerT>
	class ContainerTicket
	{
	public:
		/** Ticket constructor. FIXME: Make private. */
		ContainerTicket(size_t pos, std::shared_ptr<PlaybackProgram const> pbp);

		/**
		 * Get container data.
		 * @return Container filled with decoded data from playback program results.
		 */
		ContainerT get() const;

		/**
		 * Check whether ticket data is already available.
		 * @return Boolean value
		 */
		bool valid() const;

	private:
		size_t pos;
		std::shared_ptr<PlaybackProgram const> pbp;
	};

#ifdef __GENPYBIND__
// Explicit instantiation of template class for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	typedef PlaybackProgram::ContainerTicket<Type> _##Name##ContainerTicket GENPYBIND(opaque);
#include "fisch/vx/container.def"
#endif // __GENPYBIND__

	/**
	 * Read-ticket for multiple containers.
	 * @tparam ContainerT Container type of corresponding data
	 */
	template <class ContainerT>
	class ContainerVectorTicket
	{
	public:
		/** Ticket constructor. FIXME: Make private. */
		ContainerVectorTicket(
		    size_t container_count, size_t jtag_pos, std::shared_ptr<PlaybackProgram const> pbp);

		/**
		 * Get data of containers.
		 * @return Containers filled with decoded data from playback program results
		 */
		std::vector<ContainerT> get() const;

		/**
		 * Check whether ticket data is already available.
		 * @return Boolean value
		 */
		bool valid() const;

	private:
		size_t container_count;
		size_t pos;
		std::shared_ptr<PlaybackProgram const> pbp;
	};

#ifdef __GENPYBIND__
// Explicit instantiation of template class for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	typedef PlaybackProgram::ContainerVectorTicket<Type> _##Name##ContainerVectorTicket GENPYBIND( \
	    opaque);
#include "fisch/vx/container.def"
#endif // __GENPYBIND__

	typedef hxcomm::vx::ut_message_to_fpga_variant send_message_type GENPYBIND(hidden);
	typedef hxcomm::vx::ut_message_from_fpga_variant receive_message_type GENPYBIND(hidden);

	/**
	 * Print instruction UT messages to ostream.
	 * @return Altered ostream
	 */
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program);

	/** FIXME: make private */
	std::vector<send_message_type> const& get_send_messages() const GENPYBIND(hidden);
	void push_received_message(receive_message_type const& message) GENPYBIND(hidden);

private:
	friend class PlaybackProgramBuilder;

	std::vector<send_message_type> m_instructions;
	std::vector<hxcomm::vx::ut_message_from_fpga<hxcomm::vx::instruction::jtag_from_hicann::data> >
	    m_receive_queue_jtag;
	std::vector<hxcomm::vx::ut_message_from_fpga<hxcomm::vx::instruction::omnibus_from_fpga::data> >
	    m_receive_queue_omnibus;
};

class GENPYBIND(visible) PlaybackProgramBuilder
{
public:
	/** Default constructor. */
	PlaybackProgramBuilder();

	/**
	 * Add wait instruction blocking further execution until timer reaches specified value.
	 * @param coord Timer coordinate
	 * @param time Timer time value until to wait
	 */
	void wait_until(Timer::coordinate_type const& coord, Timer::Value time);

	/**
	 * Add write instruction for container.
	 * @tparam ContainerT Container type
	 * @param coord Container coordinate
	 * @param config Container data
	 */
	template <class ContainerT>
	void write(typename ContainerT::coordinate_type const& coord, ContainerT const& config);

	/**
	 * Add write instruction for multiple containers.
	 * @tparam ContainerT Container type
	 * @param coords Container coordinates
	 * @param configs Container data
	 */
	template <class ContainerT>
	void write(
	    std::vector<typename ContainerT::coordinate_type> const& coords,
	    std::vector<ContainerT> const& configs);

	/**
	 * Add read instruction for container.
	 * @tparam ContainerT Container type
	 * @param coord Container coordinate
	 * @return Ticket accessor to response data
	 */
	template <class ContainerT>
	PlaybackProgram::ContainerTicket<ContainerT> read(
	    typename ContainerT::coordinate_type const& coord);

	/**
	 * Add read instruction for multiple containers.
	 * @tparam ContainerT Container type
	 * @param coords Container coordinates
	 * @return Ticket accessor to response data
	 */
	template <class ContainerT>
	PlaybackProgram::ContainerVectorTicket<ContainerT> read(
	    std::vector<typename ContainerT::coordinate_type> const& coords);

	/**
	 * Add halt instruction to indicate end of playback program execution.
	 */
	void halt();

	/**
	 * Add halt instruction to indicate end of playback program execution.
	 * Add read instruction for container. This is only needed for python wrapping with non-unique
	 * coordinates for overload resolution.
	 * @tparam ContainerT Container type
	 * @param coord Container coordinate
	 * @param config Container data (unused)
	 * @return Ticket accessor to response data
	 */
	template <class ContainerT>
	PlaybackProgram::ContainerTicket<ContainerT> read(
	    typename ContainerT::coordinate_type const& coord, ContainerT const& config);

	/**
	 * Add read instruction for multiple containers. This is only needed for python wrapping with
	 * non-unique coordinates for overload resolution.
	 * @tparam ContainerT Container type
	 * @param coords Container coordinates
	 * @param config Container data (unused)
	 * @return Ticket accessor to response data
	 */
	template <class ContainerT>
	PlaybackProgram::ContainerVectorTicket<ContainerT> read(
	    std::vector<typename ContainerT::coordinate_type> const& coords, ContainerT const& config);

	/**
	 * Finish playback program creation and return built program. Resets the state of the builder.
	 * @return Built playback program
	 */
	std::shared_ptr<PlaybackProgram> done();

private:
	std::shared_ptr<PlaybackProgram> m_program;
	size_t m_jtag_receive_queue_size;
	size_t m_omnibus_receive_queue_size;
};

#ifdef __GENPYBIND__
// Explicit instantiation of template member functions for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	extern template PlaybackProgram::ContainerTicket<Name> PlaybackProgramBuilder::read<Name>(     \
	    typename Name::coordinate_type const& coord);                                              \
	extern template PlaybackProgram::ContainerVectorTicket<Name>                                   \
	PlaybackProgramBuilder::read<Name>(                                                            \
	    std::vector<typename Name::coordinate_type> const& coord, Name const& config);             \
	extern template PlaybackProgram::ContainerTicket<Name> PlaybackProgramBuilder::read<Name>(     \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	extern template PlaybackProgram::ContainerVectorTicket<Name>                                   \
	PlaybackProgramBuilder::read<Name>(std::vector<typename Name::coordinate_type> const& coord);  \
	extern template void PlaybackProgramBuilder::write<Name>(                                      \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	extern template void PlaybackProgramBuilder::write<Name>(                                      \
	    std::vector<typename Name::coordinate_type> const& coords,                                 \
	    std::vector<Name> const& configs);
#include "fisch/vx/container.def"
#endif // __GENPYBIND__

} // namespace fisch::vx
