#pragma once
#include "fisch/vx/container.h"
#include "fisch/vx/genpybind.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"
#include <memory>
#include <vector>

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgram;

template <typename ContainerT>
class ContainerTicket;

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
 * Instructions can be of type write or read and are added to the current end of the
 * instruction sequence.
 * A write is defined as an instruction without deterministic response, whereas a read is defined as
 * instruction with deterministic response expectation which is encapsulated in a ticket accessor
 * to the to-be-acquired response data.
 */
class GENPYBIND(visible) PlaybackProgramBuilder
{
public:
	/** Default constructor. */
	PlaybackProgramBuilder() SYMBOL_VISIBLE;

	~PlaybackProgramBuilder() SYMBOL_VISIBLE;

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
	ContainerTicket<typename detail::coordinate_type_to_container_type<CoordinateT>::type> read(
	    std::vector<CoordinateT> const& coords);

	/**
	 * Finish playback program creation and return built program. Resets the state of the builder.
	 * @return Built playback program
	 */
	std::shared_ptr<PlaybackProgram> done() SYMBOL_VISIBLE;

	/**
	 * Print instruction UT messages added so far to ostream.
	 * @return Altered ostream
	 */
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PlaybackProgramBuilder const& builder)
	    SYMBOL_VISIBLE;

	/**
	 * Merge other PlaybackProgramBuilder to the end of this builder instance.
	 * The moved-from builder is emptied during the process.
	 * @param other Builder to move to this instance at the back
	 */
	void merge_back(PlaybackProgramBuilder& other) SYMBOL_VISIBLE;

	/**
	 * Merge other PlaybackProgramBuilder to the beginning of this builder instance.
	 * The moved-from builder is emptied during the process.
	 * @param other Builder to move to this instance at the front
	 */
	void merge_front(PlaybackProgramBuilder& other) SYMBOL_VISIBLE;

	/**
	 * Copy other PlaybackProgramBuilder to the end of this builder instance.
	 * The copied-from builder is untouched during the process.
	 * @throws std::runtime_error On other builder not being write only
	 * @param other Builder to copy to this instance at the back
	 */
	void copy_back(PlaybackProgramBuilder const& other) SYMBOL_VISIBLE;

	/**
	 * Get whether builder is empty, i.e. no instructions are embodied.
	 * @return Boolean value
	 */
	bool empty() const SYMBOL_VISIBLE;

	/**
	 * Get whether program only contains write data.
	 * @return Boolean Value
	 */
	bool is_write_only() const SYMBOL_VISIBLE;

	/**
	 * Get number of UT messages to FPGA.
	 * @return Size
	 */
	size_t size_to_fpga() const SYMBOL_VISIBLE;

	/**
	 * Get number of UT messages from FPGA.
	 * @return Size
	 */
	size_t size_from_fpga() const SYMBOL_VISIBLE;

private:
	std::shared_ptr<PlaybackProgram> m_program;

	// intermediate buffer for instructions (moved to m_program in done())
	std::vector<hxcomm::vx::UTMessageToFPGAVariant> m_instructions;
};

// Explicit instantiation of template member functions for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, _Type)                                                            \
	extern template ContainerTicket<Name> SYMBOL_VISIBLE                                           \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    typename Name::coordinate_type const& coord);                                              \
	extern template ContainerTicket<Name> SYMBOL_VISIBLE                                           \
	PlaybackProgramBuilder::read<typename Name::coordinate_type>(                                  \
	    std::vector<typename Name::coordinate_type> const& coord);                                 \
	extern template void SYMBOL_VISIBLE PlaybackProgramBuilder::write<Name>(                       \
	    typename Name::coordinate_type const& coord, Name const& config);                          \
	extern template void SYMBOL_VISIBLE PlaybackProgramBuilder::write<Name>(                       \
	    std::vector<typename Name::coordinate_type> const& coords,                                 \
	    std::vector<Name> const& configs);
#include "fisch/vx/container.def"

} // namespace vx
} // namespace fisch
