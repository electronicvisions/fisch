#pragma once
#include "fisch/vx/container_fwd.h"
#include "fisch/vx/genpybind.h"
#include "hate/visibility.h"
#include <memory>
#include <vector>

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgramBuilder;
class PlaybackProgram;
struct FPGATime;

namespace detail {

template <typename ContainerT>
struct ContainerTicketStorage
{
	typedef ContainerT container_type;
	size_t m_pos;
	std::shared_ptr<PlaybackProgram const> m_pbp;
};

} // namespace detail

/**
 * Read-ticket for multiple containers.
 * @tparam ContainerT Container type of corresponding data
 */
template <class ContainerT>
class ContainerTicket
{
public:
	ContainerTicket() = default;

	typedef ContainerT container_type;

	/**
	 * Get data of containers.
	 * @return Containers filled with decoded data from playback program results
	 */
	std::vector<ContainerT> get() const SYMBOL_VISIBLE;

	/**
	 * Check whether ticket data is already available.
	 * @return Boolean value
	 */
	bool valid() const SYMBOL_VISIBLE;

	/**
	 * Get FPGA executor timestamp of last container response if time annotation is enabled.
	 * If time annotation is not enabled, get message count since last time annotation or
	 * from the beginning of the response stream.
	 * @return FPGATime value
	 */
	FPGATime fpga_time() const SYMBOL_VISIBLE;

	ContainerTicket(ContainerTicket const& other) = default;
	ContainerTicket& operator=(ContainerTicket const& other) = default;
	ContainerTicket(ContainerTicket&& other) = default;
	ContainerTicket& operator=(ContainerTicket&& other) = default;

private:
	ContainerTicket(
	    size_t container_count,
	    std::shared_ptr<detail::ContainerTicketStorage<ContainerT>> storage);

	friend class PlaybackProgramBuilder;

	size_t m_container_count{};
	std::shared_ptr<detail::ContainerTicketStorage<ContainerT>> m_storage;
};

// Explicit instantiation of template class for all valid playback container types.
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	extern template class SYMBOL_VISIBLE ContainerTicket<Type>;                                    \
	typedef ContainerTicket<Type> _##Name##ContainerTicket GENPYBIND(opaque);
#include "fisch/vx/container.def"

} // namespace vx
} // namespace fisch
