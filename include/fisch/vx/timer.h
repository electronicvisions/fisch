#pragma once
#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/timer.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct TimerOnDLS;
struct WaitUntilOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for writing and reading a timer value.
 */
class GENPYBIND(visible) Timer
{
public:
	typedef halco::hicann_dls::vx::TimerOnDLS coordinate_type;

	typedef word_access_type::Timer Value;

	/**
	 * Default constructor.
	 */
	Timer() : m_value() {}

	/**
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit Timer(Value const& value) : m_value(value) {}

	/**
	 * Set timer value.
	 * @param value Value to set
	 */
	void set(Value value);

	/**
	 * Get timer value.
	 * @return Value value
	 */
	Value get() const;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Timer const& timer);

	bool operator==(Timer const& other) const;
	bool operator!=(Timer const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

private:
	Value m_value;
	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

/**
 * Container for blocking further execution until timer value reaches specified value.
 */
class GENPYBIND(visible) WaitUntil
{
public:
	typedef halco::hicann_dls::vx::WaitUntilOnFPGA coordinate_type;

	typedef word_access_type::WaitUntil Value;

	/**
	 * Construct timer with value.
	 * @param value Value to construct timer with
	 */
	explicit WaitUntil(Value value = Value());

	/**
	 * Get timer value.
	 * @return Value
	 */
	Value get() const;

	/**
	 * Set timer value.
	 * @param value Value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, WaitUntil const& config);

	bool operator==(WaitUntil const& other) const;
	bool operator!=(WaitUntil const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx
