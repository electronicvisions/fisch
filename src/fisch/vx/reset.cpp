#include "fisch/vx/reset.h"

#include "fisch/cerealization.h"
#include "halco/hicann-dls/vx/reset.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

ResetChip::ResetChip(bool const value) : m_value(value) {}

bool ResetChip::get() const
{
	return m_value;
}

void ResetChip::set(bool const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, ResetChip const& reset)
{
	std::stringstream ss;
	ss << std::boolalpha << reset.m_value;
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, ResetChip::encode_write_ut_message_count>
ResetChip::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Reset>(
	    hxcomm::vx::instruction::system::Reset::Payload(m_value))};
}

template <class Archive>
void ResetChip::serialize(Archive& ar, std::uint32_t)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ResetChip)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::ResetChip, 0)
