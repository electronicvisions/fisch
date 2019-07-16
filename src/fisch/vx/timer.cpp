#include "fisch/vx/timer.h"

namespace fisch::vx {

Timer::Timer(Value const value) : m_value(value) {}

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

std::array<hxcomm::vx::UTMessageToFPGAVariant, Timer::encode_read_ut_message_count>
Timer::encode_read(coordinate_type const& /* coord */)
{
	return {};
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Timer::encode_write_ut_message_count>
Timer::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Setup>()};
}

void Timer::decode(
    std::array<hxcomm::vx::UTMessageFromFPGAVariant, Timer::decode_ut_message_count> const&
    /*messages*/)
{}

template <class Archive>
void Timer::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Timer)

} // namespace fisch::vx
