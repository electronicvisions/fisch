#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/back_emplacer_fwd.h"
#include "fisch/vx/decode.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/timer.h"
#include "hate/visibility.h"
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
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit Timer(Value const& value = Value()) : m_value(value) {}

	/**
	 * Set timer value.
	 * @param value Value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	/**
	 * Get timer value.
	 * @return Value value
	 */
	Value get() const SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Timer const& timer) SYMBOL_VISIBLE;

	bool operator==(Timer const& other) const SYMBOL_VISIBLE;
	bool operator!=(Timer const& other) const SYMBOL_VISIBLE;

	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	static void encode_read(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target)
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden) SYMBOL_VISIBLE;

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
	explicit WaitUntil(Value value = Value()) : m_value(value) {}

	/**
	 * Get timer value.
	 * @return Value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set timer value.
	 * @param value Value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, WaitUntil const& config) SYMBOL_VISIBLE;

	bool operator==(WaitUntil const& other) const SYMBOL_VISIBLE;
	bool operator!=(WaitUntil const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::Timer)
FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::WaitUntil)
