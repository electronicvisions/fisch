#pragma once
#include <mutex>
#include <unordered_set>
#include <variant>

#include "fisch/vx/genpybind.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/event.h"
#include "fisch/vx/playback_decoder.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

class PlaybackProgramBuilder;

template <typename ContainerT>
class ContainerTicket;

#define PLAYBACK_CONTAINER(Name, Type) class Name;
#include "fisch/vx/container.def"

/** Playback program. */
class GENPYBIND(visible, holder_type("std::shared_ptr<fisch::vx::PlaybackProgram>")) PlaybackProgram
    : public std::enable_shared_from_this<PlaybackProgram>
{
public:
	/** Default constructor */
	PlaybackProgram();

	~PlaybackProgram();

	typedef std::vector<SpikeFromChipEvent> spike_from_chip_events_type;
	typedef std::vector<MADCSampleFromChipEvent> madc_sample_from_chip_events_type;
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
	 * Get vector of time-annotated highspeed-link notifications.
	 * @return Vector of notifications
	 */
	GENPYBIND(getter_for(highspeed_link_notifications))
	highspeed_link_notifications_type const& get_highspeed_link_notifications() const;

	/**
	 * Check that result data to all tickets is available.
	 * @return Boolean value
	 */
	bool valid() const;

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

	/**
	 * Get whether program is empty, i.e. no instructions are embodied.
	 * @return Boolean value
	 */
	bool empty() const;

	bool operator==(PlaybackProgram const& other) const;
	bool operator!=(PlaybackProgram const& other) const;

private:
	friend class PlaybackProgramBuilder;

	template <typename T>
	friend class ContainerTicket;

	template <typename U>
	void register_ticket(U* const ticket) const;

	template <typename U>
	void deregister_ticket(U* const ticket) const;

#define LAST_PLAYBACK_CONTAINER(Name, Type) ContainerTicket<Type>*
#define PLAYBACK_CONTAINER(Name, Type) LAST_PLAYBACK_CONTAINER(Name, Type),
	mutable std::unordered_set<std::variant<
#include "fisch/vx/container.def"
	    >>
	    m_tickets;

	mutable std::mutex m_tickets_mutex;

	std::vector<to_fpga_message_type> m_instructions;

	PlaybackDecoder::response_queue_type m_receive_queue;
	PlaybackDecoder::spike_queue_type m_spike_response_queue;
	PlaybackDecoder::madc_sample_queue_type m_madc_sample_response_queue;
	PlaybackDecoder::highspeed_link_notification_queue_type
	    m_highspeed_link_notification_response_queue;
	PlaybackDecoder::spike_pack_counts_type m_spike_pack_counts;
	PlaybackDecoder::madc_sample_pack_counts_type m_madc_sample_pack_counts;

	PlaybackDecoder m_decoder;
	std::array<size_t, std::tuple_size<decltype(m_receive_queue)>::value> m_queue_expected_size;

	friend class cereal::access;
	template <typename Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx
