#include "fisch/vx/systime.h"

#include "fisch/cerealization.h"
#include "halco/hicann-dls/vx/timing.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

SystimeSync::SystimeSync(bool const do_sync) : m_do_sync(do_sync) {}

bool SystimeSync::get() const
{
	return m_do_sync;
}

void SystimeSync::set(bool const value)
{
	m_do_sync = value;
}

std::ostream& operator<<(std::ostream& os, SystimeSync const& systime_sync)
{
	std::stringstream ss;
	os << "SystimeSync(" << (systime_sync.m_do_sync ? "true" : "false") << ")";
	return os;
}

bool SystimeSync::operator==(SystimeSync const& other) const
{
	return (m_do_sync == other.m_do_sync);
}

bool SystimeSync::operator!=(SystimeSync const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, SystimeSync::encode_write_ut_message_count>
SystimeSync::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::SystimeInit>(
	    hxcomm::vx::instruction::timing::SystimeInit::Payload(m_do_sync))};
}

template <class Archive>
void SystimeSync::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_do_sync));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SystimeSync)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::SystimeSync, 0)
