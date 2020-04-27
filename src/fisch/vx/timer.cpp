#include "fisch/vx/timer.h"

#include "fisch/cerealization.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/timing.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

Timer::Timer(Value const value) : m_value()
{
	set(value);
}

Timer::Value Timer::get() const
{
	return m_value;
}

void Timer::set(Value const value)
{
	if (value.value() != 0) {
		throw std::runtime_error("Setting Timer to value different from 0 not supported for now.");
	}
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, Timer const& timer)
{
	std::stringstream ss;
	ss << std::dec << timer.m_value.value();
	os << "Timer(" << ss.str() << ")";
	return os;
}

bool Timer::operator==(Timer const& other) const
{
	return (m_value == other.m_value);
}

bool Timer::operator!=(Timer const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Timer::encode_write_ut_message_count>
Timer::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Setup>()};
}

template <class Archive>
void Timer::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Timer)


WaitUntil::WaitUntil(Value const value) : m_value(value) {}

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

std::array<hxcomm::vx::UTMessageToFPGAVariant, WaitUntil::encode_write_ut_message_count>
WaitUntil::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::WaitUntil>(
	    hxcomm::vx::instruction::timing::WaitUntil::Payload(m_value))};
}

template <class Archive>
void WaitUntil::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(WaitUntil)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Timer, 0)
CEREAL_CLASS_VERSION(fisch::vx::WaitUntil, 0)
