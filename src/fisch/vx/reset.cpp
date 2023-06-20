#include "fisch/vx/reset.h"

#include "fisch/vx/encode.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

ResetChip::Value ResetChip::get() const
{
	return m_value;
}

void ResetChip::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, ResetChip const& reset)
{
	std::stringstream ss;
	ss << std::boolalpha << reset.m_value.value();
	os << "ResetChip(" << ss.str() << ")";
	return os;
}

bool ResetChip::operator==(ResetChip const& other) const
{
	return (m_value == other.m_value);
}

bool ResetChip::operator!=(ResetChip const& other) const
{
	return !(*this == other);
}

void ResetChip::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	return target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Reset>(
	    hxcomm::vx::instruction::system::Reset::Payload(m_value)));
}

} // namespace fisch::vx
