#include "fisch/vx/playback_decoder.h"

namespace fisch::vx {

PlaybackDecoder::PlaybackDecoder(jtag_queue_type& jtag_queue, omnibus_queue_type& omnibus_queue) :
    m_jtag_queue(jtag_queue),
    m_omnibus_queue(omnibus_queue)
{}

void PlaybackDecoder::operator()(ut_message_from_fpga_variant_type const& message)
{
	boost::apply_visitor([this](auto m) { process(m); }, message);
}

void PlaybackDecoder::process(ut_message_from_fpga_jtag_type const& message)
{
	m_jtag_queue.push_back(message);
}

void PlaybackDecoder::process(ut_message_from_fpga_omnibus_type const& message)
{
	m_omnibus_queue.push_back(message);
}

void PlaybackDecoder::process(ut_message_from_fpga_halt_type const&)
{ /* do nothing */
}

} // namespace fisch::vx
