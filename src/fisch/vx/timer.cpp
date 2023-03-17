#include "fisch/vx/timer.h"
#include "fisch/vx/omnibus.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

void Timer::set(Value const value)
{
	m_data = value;
}

Timer::Value Timer::get() const
{
	return m_data;
}

std::ostream& operator<<(std::ostream& os, Timer const& timer)
{
	os << "Timer(" << timer.m_data.value() << ")";
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, Timer::encode_write_ut_message_count>
Timer::encode_write(coordinate_type const& /* coord */) const
{
	if (m_data != Value(0)) {
		throw std::runtime_error("Trying to write value different from 0. This is not supported!");
	}
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Setup>()};
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Timer::encode_read_ut_message_count>
Timer::encode_read(coordinate_type const& /* coord */)
{
	return Omnibus::encode_read(halco::hicann_dls::vx::OmnibusAddress(timer_readout));
}


void Timer::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	Omnibus word;
	word.decode(messages);
	auto value = word.get().word.value();

	m_data = Value(value);
}

template <class Archive>
void Timer::serialize(Archive&, std::uint32_t const)
{}

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

CEREAL_CLASS_VERSION(fisch::vx::Timer, 1)
CEREAL_CLASS_VERSION(fisch::vx::WaitUntil, 0)
