#include "fisch/vx/detail/playback_program_impl.h"

#include "cereal/types/halco/common/geometry.h"
#include "cereal/types/halco/common/typed_array.h"
#include "cereal/types/hxcomm/common/utmessage.h"
#include "fisch/cerealization.tcc"
#include "fisch/vx/event.h"
#include "fisch/vx/fpga_time.h"
#include "fisch/vx/playback_decoder.h"
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
