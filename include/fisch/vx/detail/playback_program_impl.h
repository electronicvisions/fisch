#pragma once
#include "fisch/vx/playback_decoder.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"
#include <array>
#include <cstdint>
#include <forward_list>
#include <mutex>
#include <variant>
#include <vector>


namespace fisch::vx {

namespace detail {
template <typename ContainerT>
class ContainerTicketStorage;
} // namespace detail

#define PLAYBACK_CONTAINER(Name, Type) class Name;
#include "fisch/vx/container.def"

} // namespace fisch::vx

namespace fisch::vx::detail {

struct PlaybackProgramImpl
{
#define LAST_PLAYBACK_CONTAINER(Name, Type) std::weak_ptr<detail::ContainerTicketStorage<Type>>
#define PLAYBACK_CONTAINER(Name, Type) LAST_PLAYBACK_CONTAINER(Name, Type),
	/**
	 * @note Collection of message types omitted in documentation.
	 */
	mutable std::forward_list<std::variant<
/// @cond DISABLED
#include "fisch/vx/container.def"
	    /// @endcond
	    >>
	    m_tickets;

	mutable std::mutex m_tickets_mutex;

	std::vector<hxcomm::vx::UTMessageToFPGAVariant> m_instructions;

	PlaybackDecoder::response_queue_type m_receive_queue;
	PlaybackDecoder::spike_queue_type m_spike_response_queue;
	PlaybackDecoder::madc_sample_queue_type m_madc_sample_response_queue;
	PlaybackDecoder::highspeed_link_notification_queue_type
	    m_highspeed_link_notification_response_queue;
	PlaybackDecoder::spike_pack_counts_type m_spike_pack_counts;
	PlaybackDecoder::madc_sample_pack_counts_type m_madc_sample_pack_counts;
	PlaybackDecoder::timeout_notification_queue_type m_timeout_notification_response_queue;

	PlaybackDecoder m_decoder;
	std::array<size_t, std::tuple_size<decltype(m_receive_queue)>::value> m_queue_expected_size;

	PlaybackProgramImpl() SYMBOL_VISIBLE;

	bool operator==(PlaybackProgramImpl const& other) const;
	bool operator!=(PlaybackProgramImpl const& other) const;

	template <typename Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx::detail
