#pragma once
#include <variant>

#include "fisch/vx/container.h"
#include "fisch/vx/genpybind.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/event.h"
#include "fisch/vx/playback_decoder.h"
#include "fisch/vx/systime.h"

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
		typedef ContainerT container_type;

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

		/**
		 * Get FPGA executor timestamp of last response message if time annotation is enabled.
		 * If time annotation is not enabled, get message count since last time annotation or
		 * from the beginning of the response stream.
		 * @return FPGATime value
		 */
		FPGATime fpga_time() const;

		ContainerTicket(ContainerTicket const& other);
		~ContainerTicket();

	private:
		ContainerTicket(size_t pos, std::shared_ptr<PlaybackProgram const> pbp);

		friend class PlaybackProgramBuilder;

		size_t m_pos;
		std::shared_ptr<PlaybackProgram const> m_pbp;
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
		typedef ContainerT container_type;

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

		/**
		 * Get FPGA executor timestamp of last container response if time annotation is enabled.
		 * If time annotation is not enabled, get message count since last time annotation or
		 * from the beginning of the response stream.
		 * @return FPGATime value
		 */
		FPGATime fpga_time() const;

		ContainerVectorTicket(ContainerVectorTicket const& other);
		~ContainerVectorTicket();

	private:
		ContainerVectorTicket(
		    size_t container_count, size_t pos, std::shared_ptr<PlaybackProgram const> pbp);

		friend class PlaybackProgramBuilder;

		size_t m_container_count;
		size_t m_pos;
		std::shared_ptr<PlaybackProgram const> m_pbp;
	};

#ifdef __GENPYBIND__
// Explicit instantiation of template class for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	typedef PlaybackProgram::ContainerVectorTicket<Type> _##Name##ContainerVectorTicket GENPYBIND( \
	    opaque);
#include "fisch/vx/container.def"
#endif // __GENPYBIND__

	/** Default constructor */
	PlaybackProgram();

	typedef std::vector<SpikeFromChipEvent> spike_from_chip_events_type;
	typedef std::vector<MADCSampleFromChipEvent> madc_sample_from_chip_events_type;

	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::SpikePackFromFPGAOnDLS>
	    spike_pack_counts_type GENPYBIND(opaque(false));
	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::MADCSamplePackFromFPGAOnDLS>
	    madc_sample_pack_counts_type GENPYBIND(opaque(false));

	/**
	 * Get number of occurences of spike packing from chip.
	 * @return Array of packing occurences
	 */
	GENPYBIND(getter_for(spikes_pack_counts))
	spike_pack_counts_type const& get_spikes_pack_counts() const;

	/**
	 * Get number of occurences of MADC sample packing from chip.
	 * @return Array of packing occurences
	 */
	GENPYBIND(getter_for(madc_samples_pack_counts))
	madc_sample_pack_counts_type const& get_madc_samples_pack_counts() const;

	/**
	 * Get vector of time-annotated spike events.
	 * @return Vector of spike events
	 */
	GENPYBIND(getter_for(spikes))
	spike_from_chip_events_type const& get_spikes() const;

	/**
	 * Get vector of time-annotated MADC sample events.
	 * @return Vector of sample events
	 */
	GENPYBIND(getter_for(madc_samples))
	madc_sample_from_chip_events_type const& get_madc_samples() const;

	/**
	 * Print instruction UT messages to ostream.
	 * @return Altered ostream
	 */
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program);

	typedef hxcomm::vx::UTMessageToFPGAVariant to_fpga_message_type;
	typedef hxcomm::vx::UTMessageFromFPGAVariant from_fpga_message_type;

	/**
	 * Get to FPGA instruction sequence.
	 * @return Vector of UT messages
	 */
	std::vector<to_fpga_message_type> const& get_to_fpga_messages() const GENPYBIND(hidden);

	/**
	 * Push a UT message from FPGA into the response decoder.
	 * The message is processed and maybe put into a data response queue.
	 * @param message UT message to push into the decoder
	 */
	void push_from_fpga_message(from_fpga_message_type const& message) GENPYBIND(hidden);

	/**
	 * Clear the content of response data queues in order to refill them through a successive
	 * execution.
	 */
	void clear_from_fpga_messages();

	bool operator==(PlaybackProgram const& other) const;
	bool operator!=(PlaybackProgram const& other) const;

private:
	friend class PlaybackProgramBuilder;

	template <typename U>
	void register_ticket(U* const ticket) const;

	template <typename U>
	void deregister_ticket(U* const ticket) const;

#define LAST_PLAYBACK_CONTAINER(Name, Type) ContainerTicket<Type>*, ContainerVectorTicket<Type>*
#define PLAYBACK_CONTAINER(Name, Type) LAST_PLAYBACK_CONTAINER(Name, Type),
	mutable std::unordered_set<std::variant<
#include "fisch/vx/container.def"
	    >>
	    m_tickets;

	std::vector<to_fpga_message_type> m_instructions;

	PlaybackDecoder::jtag_queue_type m_receive_queue_jtag;
	PlaybackDecoder::omnibus_queue_type m_receive_queue_omnibus;
	PlaybackDecoder::spike_queue_type m_spike_response_queue;
	PlaybackDecoder::madc_sample_queue_type m_madc_sample_response_queue;
	PlaybackDecoder::spike_pack_counts_type m_spike_pack_counts;
	PlaybackDecoder::madc_sample_pack_counts_type m_madc_sample_pack_counts;

	PlaybackDecoder m_decoder;
};


namespace detail {

#define LAST_PLAYBACK_CONTAINER(Name, Type) Type
#define PLAYBACK_CONTAINER(Name, Type) LAST_PLAYBACK_CONTAINER(Name, Type),
/**
 * List of container types used to resolve a container type from a coordinate type.
 */
typedef hate::type_list<
#include "fisch/vx/container.def"
    >
    container_list;

#define LAST_PLAYBACK_CONTAINER(Name, Type) typename Type::coordinate_type
#define PLAYBACK_CONTAINER(Name, Type) LAST_PLAYBACK_CONTAINER(Name, Type),
/**
 * List of coordinate types sorted the same way as the container list.
 */
typedef hate::type_list<
#include "fisch/vx/container.def"
    >
    coordinate_list;

/**
 * Given a coordinate type, resolves the corresponding unique container type.
 * @tparam CoordinateT Coordinate type to resolve container type for
 */
template <typename CoordinateT>
struct coordinate_type_to_container_type
{
	typedef typename hate::index_type_list_by_integer<
	    hate::index_type_list_by_type<CoordinateT, coordinate_list>::value,
	    container_list>::type type;
};

/**
 * Specialization for vector<coordinate>.
 * The struct has to resolve a container type for vector<coordinate>, because it is always
 * evaluated, even if the fully specified read function signature does not match.
 */
template <typename CoordinateT>
struct coordinate_type_to_container_type<std::vector<CoordinateT>>
{
	typedef void type; /// void because it is never used
};

} // namespace detail


/**
 * Sequential playback program builder.
 * It allows building a (timed) sequence of logical instructions to be executed on the FPGA.
 * Instructions can be of type write, read and wait_until and are added to the current end of the
 * instruction sequence.
 * A write is defined as an instruction without deterministic response, whereas a read is defined as
 * instruction with deterministic response expectation which is encapsulated in a ticket accessor
 * to the to-be-acquired response data.
 */
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
	 * @tparam CoordinateT Coordinate type
	 * @param coord Container coordinate
	 * @return Ticket accessor to response data
	 */
	template <class CoordinateT>
	PlaybackProgram::ContainerTicket<
	    typename detail::coordinate_type_to_container_type<CoordinateT>::type>
	read(CoordinateT const& coord);

	/**
	 * Add read instruction for multiple containers.
	 * @tparam ContainerT Container type
	 * @param coords Container coordinates
	 * @return Ticket accessor to response data
	 */
	template <class CoordinateT>
	PlaybackProgram::ContainerVectorTicket<
	    typename detail::coordinate_type_to_container_type<CoordinateT>::type>
	read(std::vector<CoordinateT> const& coords);

	/**
	 * Finish playback program creation and return built program. Resets the state of the builder.
	 * @return Built playback program
	 */
	std::shared_ptr<PlaybackProgram> done();

	/**
	 * Print instruction UT messages added so far to ostream.
	 * @return Altered ostream
	 */
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PlaybackProgramBuilder const& builder);

	/**
	 * Merge other PlaybackProgramBuilder to the end of this builder instance.
	 * The moved-from builder is emptied during the process.
	 * @param other Builder to move to this instance at the back
	 */
	void merge_back(PlaybackProgramBuilder& other);

private:
	std::shared_ptr<PlaybackProgram> m_program;
	size_t m_jtag_receive_queue_size;
	size_t m_omnibus_receive_queue_size;
};

#ifdef __GENPYBIND__
// Explicit instantiation of template member functions for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	extern template PlaybackProgram::ContainerTicket<Name>                                         \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    typename Name::coordinate_type const& coord);                                              \
	extern template PlaybackProgram::ContainerVectorTicket<Name>                                   \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    std::vector<typename Name::coordinate_type> const& coord);                                 \
	extern template void PlaybackProgramBuilder::write<Name>(                                      \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	extern template void PlaybackProgramBuilder::write<Name>(                                      \
	    std::vector<typename Name::coordinate_type> const& coords,                                 \
	    std::vector<Name> const& configs);
#include "fisch/vx/container.def"
#endif // __GENPYBIND__

} // namespace fisch::vx
