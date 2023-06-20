#include "fisch/vx/systime.h"

#include "fisch/vx/encode.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

SystimeSync::Value SystimeSync::get() const
{
	return m_value;
}

void SystimeSync::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, SystimeSync const& systime_sync)
{
	std::stringstream ss;
	os << "SystimeSync(" << (systime_sync.m_value ? "true" : "false") << ")";
	return os;
}

bool SystimeSync::operator==(SystimeSync const& other) const
{
	return (m_value == other.m_value);
}

bool SystimeSync::operator!=(SystimeSync const& other) const
{
	return !(*this == other);
}

void SystimeSync::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::SystimeInit>(
	    hxcomm::vx::instruction::timing::SystimeInit::Payload(m_value)));
}

} // namespace fisch::vx
