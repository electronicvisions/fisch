#pragma once
#include <memory>
#include <vector>

#include "fisch/vx/genpybind.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgramBuilder;
class PlaybackProgram;
struct FPGATime;

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
	ContainerTicket& operator=(ContainerTicket const& other) = default;
	~ContainerTicket();

private:
	ContainerTicket(size_t pos, std::shared_ptr<PlaybackProgram const> pbp);

	friend class PlaybackProgramBuilder;

	size_t m_pos;
	std::shared_ptr<PlaybackProgram const> m_pbp;
};


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
	ContainerVectorTicket& operator=(ContainerVectorTicket const& other) = default;
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
	extern template class ContainerTicket<Type>;                                                   \
	extern template class ContainerVectorTicket<Type>;                                             \
	typedef ContainerTicket<Type> _##Name##ContainerTicket GENPYBIND(opaque);                      \
	typedef ContainerVectorTicket<Type> _##Name##ContainerVectorTicket GENPYBIND(opaque);
#include "fisch/vx/container.def"
#endif // __GENPYBIND__

} // namespace fisch::vx
