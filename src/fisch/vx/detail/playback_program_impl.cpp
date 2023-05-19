#include "fisch/vx/detail/playback_program_impl.h"

#include "fisch/cerealization.tcc"
#include "fisch/vx/event.h"
#include "fisch/vx/fpga_time.h"
#include "fisch/vx/playback_decoder.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/common/cerealization_typed_array.h"
#include "hxcomm/common/cerealization_utmessage.h"
#include "hxcomm/vx/utmessage.h"
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

namespace cereal {

template <typename Archive, typename MessageT>
void CEREAL_SAVE_FUNCTION_NAME(
    Archive& ar,
    fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT> const& queue,
    std::uint32_t const)
{
	auto const& messages = queue.get_messages();
	auto const& times = queue.get_times();
	ar(CEREAL_NVP(messages));
	ar(CEREAL_NVP(times));
}

template <typename Archive, typename MessageT>
void CEREAL_LOAD_FUNCTION_NAME(
    Archive& ar,
    fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT>& queue,
    std::uint32_t const)
{
	std::vector<MessageT> messages;
	std::vector<fisch::vx::FPGATime> times;
	ar(CEREAL_NVP(messages));
	ar(CEREAL_NVP(times));
	queue = fisch::vx::PlaybackDecoder::TimedResponseQueue<MessageT>(messages, times);
}

} // namespace cereal

namespace fisch::vx::detail {

PlaybackProgramImpl::PlaybackProgramImpl() :
    m_tickets(),
    m_tickets_mutex(),
    m_instructions(),
    m_receive_queue(),
    m_spike_response_queue(),
    m_madc_sample_response_queue(),
    m_highspeed_link_notification_response_queue(),
    m_spike_pack_counts(),
    m_madc_sample_pack_counts(),
    m_timeout_notification_response_queue(),
    m_decoder(
        m_receive_queue,
        m_spike_response_queue,
        m_madc_sample_response_queue,
        m_highspeed_link_notification_response_queue,
        m_spike_pack_counts,
        m_madc_sample_pack_counts,
        m_timeout_notification_response_queue),
    m_queue_expected_size()
{
	m_queue_expected_size.fill(0);
}

bool PlaybackProgramImpl::operator==(PlaybackProgramImpl const& other) const
{
	return m_instructions == other.m_instructions && m_receive_queue == other.m_receive_queue &&
	       m_spike_response_queue == other.m_spike_response_queue &&
	       m_madc_sample_response_queue == other.m_madc_sample_response_queue &&
	       m_highspeed_link_notification_response_queue ==
	           other.m_highspeed_link_notification_response_queue &&
	       m_spike_pack_counts == other.m_spike_pack_counts &&
	       m_madc_sample_pack_counts == other.m_madc_sample_pack_counts &&
	       m_timeout_notification_response_queue == other.m_timeout_notification_response_queue &&
	       m_queue_expected_size == other.m_queue_expected_size;
}

bool PlaybackProgramImpl::operator!=(PlaybackProgramImpl const& other) const
{
	return !(*this == other);
}

template <typename Archive>
void PlaybackProgramImpl::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_instructions));
	boost::hana::for_each(m_receive_queue, [&ar](auto& queue) { ar(CEREAL_NVP(queue)); });
	ar(CEREAL_NVP(m_spike_response_queue));
	ar(CEREAL_NVP(m_madc_sample_response_queue));
	ar(CEREAL_NVP(m_highspeed_link_notification_response_queue));
	ar(CEREAL_NVP(m_spike_pack_counts));
	ar(CEREAL_NVP(m_madc_sample_pack_counts));
	ar(CEREAL_NVP(m_queue_expected_size));
	ar(CEREAL_NVP(m_timeout_notification_response_queue));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PlaybackProgramImpl)

} // namespace fisch::vx::detail

CEREAL_CLASS_VERSION(fisch::vx::detail::PlaybackProgramImpl, 0)
