#include "fisch/vx/systime.h"

#include "fisch/cerealization.h"
#include "halco/common/cerealization_geometry.h"
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, SystimeSync::encode_write_ut_message_count>
SystimeSync::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::SystimeInit>(
	    hxcomm::vx::instruction::timing::SystimeInit::Payload(m_value))};
}

template <class Archive>
void SystimeSync::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SystimeSync)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::SystimeSync, 1)
