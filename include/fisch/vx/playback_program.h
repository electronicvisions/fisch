#pragma once

#include "fisch/vx/timer.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

class PlaybackProgramBuilder;

/** Playback program. */
class PlaybackProgram : public std::enable_shared_from_this<PlaybackProgram>
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
		ContainerTicket(size_t jtag_pos, std::shared_ptr<PlaybackProgram const> pbp);

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
		size_t jtag_pos;
		std::shared_ptr<PlaybackProgram const> pbp;
	};

	/**
	 * Read-ticket for multiple containers.
	 * @tparam ContainerT Container type of corresponding data
	 */
	template <class ContainerT>
	struct ContainerVectorTicket
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
		size_t jtag_pos;
		std::shared_ptr<PlaybackProgram const> pbp;
	};

	typedef hxcomm::vx::ut_message_to_fpga_variant send_message_type;
	typedef hxcomm::vx::ut_message_from_fpga_variant receive_message_type;

	/**
	 * Print instruction UT messages to ostream.
	 * @return Altered ostream
	 */
	friend std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program);

	/** FIXME: make private */
	std::vector<send_message_type> const& get_send_messages() const;
	void push_received_message(receive_message_type const& message);

private:
	friend class PlaybackProgramBuilder;

	std::vector<send_message_type> m_instructions;
	std::vector<hxcomm::vx::ut_message_from_fpga<hxcomm::vx::instruction::jtag_from_hicann::data> >
	    m_receive_queue_jtag;
};

class PlaybackProgramBuilder
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
	 * Finish playback program creation and return built program. Resets the state of the builder.
	 * @return Built playback program
	 */
	std::shared_ptr<PlaybackProgram> done();

private:
	std::shared_ptr<PlaybackProgram> m_program;
	size_t m_jtag_receive_queue_size;
};

} // namespace fisch::vx
