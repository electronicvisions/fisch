#pragma once
#include <memory>
#include <vector>

#include "fisch/vx/container.h"
#include "fisch/vx/genpybind.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgram;

template <typename ContainerT>
class ContainerTicket;

template <typename ContainerT>
class ContainerVectorTicket;

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
	ContainerTicket<typename detail::coordinate_type_to_container_type<CoordinateT>::type> read(
	    CoordinateT const& coord);

	/**
	 * Add read instruction for multiple containers.
	 * @tparam ContainerT Container type
	 * @param coords Container coordinates
	 * @return Ticket accessor to response data
	 */
	template <class CoordinateT>
	ContainerVectorTicket<typename detail::coordinate_type_to_container_type<CoordinateT>::type>
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

	/**
	 * Get whether builder is empty, i.e. no instructions are embodied.
	 * @return Boolean value
	 */
	bool empty() const;

private:
	std::shared_ptr<PlaybackProgram> m_program;
	size_t m_jtag_receive_queue_size;
	size_t m_omnibus_receive_queue_size;
};

#ifdef __GENPYBIND__
// Explicit instantiation of template member functions for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	extern template ContainerTicket<Name>                                                          \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    typename Name::coordinate_type const& coord);                                              \
	extern template ContainerVectorTicket<Name>                                                    \
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
