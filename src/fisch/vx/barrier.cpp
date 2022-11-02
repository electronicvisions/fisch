#include "fisch/vx/barrier.h"

#include "fisch/cerealization.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/barrier.h"
#include "hate/bitset.h"
#include "hate/join.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

Barrier::Barrier(Value const value) : m_value(value) {}

Barrier::Value Barrier::get() const
{
	return m_value;
}

void Barrier::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, Barrier const& config)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << config.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << config.m_value.value();
	hate::bitset<5> bits(config.m_value.value());
	os << "Barrier(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool Barrier::operator==(Barrier const& other) const
{
	return m_value == other.m_value;
}

bool Barrier::operator!=(Barrier const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Barrier::encode_write_ut_message_count>
Barrier::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Barrier>(
	    hxcomm::vx::instruction::timing::Barrier::Payload(m_value))};
}

template <class Archive>
void Barrier::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Barrier)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Barrier, 0)
