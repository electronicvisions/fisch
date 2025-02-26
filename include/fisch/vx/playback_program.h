#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/event.h"
#include "fisch/vx/genpybind.h"
#include "halco/common/typed_array.h"
#include "halco/hicann-dls/vx/event.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"
#include <memory>
#include <unordered_set>
#include <variant>

namespace cereal {
struct access;
} // namespace cereal

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgramBuilder;

namespace detail {
template <typename ContainerT>
struct ContainerTicketStorage;
} // namespace detail

template <typename ContainerT>
class ContainerTicket;

#define PLAYBACK_CONTAINER(Name, Type) class Name;
#include "fisch/vx/container.def"

namespace detail {
struct PlaybackProgramImpl;
} // namespace detail

/** Playback program. */
class GENPYBIND(visible, holder_type("std::shared_ptr<fisch::vx::PlaybackProgram>")) PlaybackProgram
    : public std::enable_shared_from_this<PlaybackProgram>
{
public:
	/** Default constructor */
	PlaybackProgram() SYMBOL_VISIBLE;

	~PlaybackProgram() SYMBOL_VISIBLE;

	typedef std::vector<SpikeFromChip> spike_from_chip_events_type;
	typedef std::vector<MADCSampleFromChip> madc_sample_from_chip_events_type;
	typedef std::vector<HighspeedLinkNotification> highspeed_link_notifications_type;

	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::SpikePackFromFPGAOnDLS>
	    spike_pack_counts_type GENPYBIND(opaque(false));
	typedef halco::common::typed_array<size_t, halco::hicann_dls::vx::MADCSamplePackFromFPGAOnDLS>
	    madc_sample_pack_counts_type GENPYBIND(opaque(false));

	/**
	 * Get number of occurences of spike packing from chip.
	 * @return Array of packing occurences
	 */
	GENPYBIND(getter_for(spikes_pack_counts))
	spike_pack_counts_type const& get_spikes_pack_counts() const SYMBOL_VISIBLE;

	/**
	 * Get number of occurences of MADC sample packing from chip.
	 * @return Array of packing occurences
	 */
	GENPYBIND(getter_for(madc_samples_pack_counts))
	madc_sample_pack_counts_type const& get_madc_samples_pack_counts() const SYMBOL_VISIBLE;

	/**
	 * Get vector of time-annotated spike events.
	 * @return Vector of spike events
	 */
	GENPYBIND(getter_for(spikes))
	spike_from_chip_events_type const& get_spikes() const SYMBOL_VISIBLE;

	/**
	 * Get vector of time-annotated MADC sample events.
	 * @return Vector of sample events
	 */
	GENPYBIND(getter_for(madc_samples))
	madc_sample_from_chip_events_type const& get_madc_samples() const SYMBOL_VISIBLE;

	/**
	 * Get vector of time-annotated highspeed-link notifications.
	 * @return Vector of notifications
	 */
	GENPYBIND(getter_for(highspeed_link_notifications))
	highspeed_link_notifications_type const& get_highspeed_link_notifications() const
	    SYMBOL_VISIBLE;

	/**
	 * Check if we did not receive fatal error notifications from the FPGA.
	 * @return Boolean value (true if ok)
	 */
	bool run_ok() const SYMBOL_VISIBLE;

	/**
	 * Check that result data to all tickets is available.
	 * @return Boolean value
	 */
	bool tickets_valid() const SYMBOL_VISIBLE;

	/**
	 * Print instruction UT messages to ostream.
	 * @return Altered ostream
	 */
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PlaybackProgram const& program)
	    SYMBOL_VISIBLE;

	typedef hxcomm::vx::UTMessageToFPGAVariant to_fpga_message_type;
	typedef hxcomm::vx::UTMessageFromFPGAVariant from_fpga_message_type;

	/**
	 * Get to FPGA instruction sequence.
	 * @return Vector of UT messages
	 */
	std::vector<to_fpga_message_type> const& get_to_fpga_messages() const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

	/**
	 * Push a UT message from FPGA into the response decoder.
	 * The message is processed and maybe put into a data response queue.
	 * @param message UT message to push into the decoder
	 */
	void push_from_fpga_message(from_fpga_message_type const& message)
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

	/**
	 * Clear the content of response data queues in order to refill them through a successive
	 * execution.
	 */
	void clear_from_fpga_messages() SYMBOL_VISIBLE;

	/**
	 * Get whether program is empty, i.e. no instructions are embodied.
	 * @return Boolean value
	 */
	bool empty() const SYMBOL_VISIBLE;

	bool operator==(PlaybackProgram const& other) const SYMBOL_VISIBLE;
	bool operator!=(PlaybackProgram const& other) const SYMBOL_VISIBLE;

private:
	friend class ReinitStackEntry;

	friend class PlaybackProgramBuilder;

	template <typename T>
	friend class ContainerTicket;

	std::unique_ptr<detail::PlaybackProgramImpl> m_impl;

	friend struct cereal::access;
	template <typename Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace vx
} // namespace fisch

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::PlaybackProgram)
