#include "fisch/vx/detail/playback_program_impl.h"

#include "fisch/vx/event.h"
#include "fisch/vx/fpga_time.h"
#include "fisch/vx/playback_decoder.h"
#include "hxcomm/vx/utmessage.h"
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>

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

} // namespace fisch::vx::detail
