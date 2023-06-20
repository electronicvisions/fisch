#include "fisch/vx/barrier.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/barrier.h"
#include "hate/bitset.h"
#include "hate/join.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

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

void Barrier::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Barrier>(
	    hxcomm::vx::instruction::timing::Barrier::Payload(m_value)));
}

} // namespace fisch::vx
