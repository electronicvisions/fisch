#include "fisch/vx/timer.h"
#include "fisch/vx/omnibus.h"

#include "fisch/vx/encode.h"
#include "fisch/vx/omnibus_constants.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

void Timer::set(Value const value)
{
	m_value = value;
}

Timer::Value Timer::get() const
{
	return m_value;
}

std::ostream& operator<<(std::ostream& os, Timer const& timer)
{
	os << "Timer(" << timer.m_value.value() << ")";
	return os;
}

bool Timer::operator==(Timer const&) const
{
	return true;
}

bool Timer::operator!=(Timer const& other) const
{
	return !(*this == other);
}

void Timer::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	if (m_value != Value(0)) {
		throw std::runtime_error("Trying to write value different from 0. This is not supported!");
	}
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Setup>());
}

void Timer::encode_read(coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target)
{
	Omnibus::encode_read(halco::hicann_dls::vx::OmnibusAddress(timer_readout), target);
}


void Timer::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	Omnibus word;
	word.decode(messages);
	auto value = word.get().word.value();

	m_value = Value(value);
}


WaitUntil::Value WaitUntil::get() const
{
	return m_value;
}

void WaitUntil::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, WaitUntil const& config)
{
	std::stringstream ss;
	ss << std::dec << config.m_value.value();
	os << "WaitUntil(" << ss.str() << ")";
	return os;
}

bool WaitUntil::operator==(WaitUntil const& other) const
{
	return (m_value == other.m_value);
}

bool WaitUntil::operator!=(WaitUntil const& other) const
{
	return !(*this == other);
}

void WaitUntil::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::WaitUntil>(
	    hxcomm::vx::instruction::timing::WaitUntil::Payload(m_value)));
}

} // namespace fisch::vx
