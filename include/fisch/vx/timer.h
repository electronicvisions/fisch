#pragma once

#include "halco/common/geometry.h"
#include "halco/common/relations.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/genpybind.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for writing a timer value.
 */
class GENPYBIND(visible) Timer
{
public:
	typedef halco::hicann_dls::vx::TimerOnDLS coordinate_type;

	/** Value type. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::BaseType<Value, uint32_t>
	{
		Value(uintmax_t value = 0) GENPYBIND(implicit_conversion) : base_t(value) {}
	};

	/** Default constructor. */
	Timer();

	/**
	 * Construct timer with value.
	 * @param value Value to construct timer with
	 */
	Timer(Value value);

	/**
	 * Get timer value.
	 * @return Value
	 */
	Value get() const;

	/**
	 * Set timer value. Currently only reset to 0 is supported.
	 * @param value Value to set timer to
	 */
	void set(Value const& value);

	bool operator==(Timer const& other) const;
	bool operator!=(Timer const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};

} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::Timer::Value)
} // namespace std
