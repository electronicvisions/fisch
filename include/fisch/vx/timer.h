#pragma once

#include "halco/common/geometry.h"
#include "halco/common/relations.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "hxcomm/vx/utmessage.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx {

/**
 * Container for writing a timer value.
 */
class Timer
{
public:
	typedef halco::hicann_dls::vx::TimerOnDLS coordinate_type;

	/** Value type. */
	struct Value : public halco::common::detail::BaseType<Value, uint32_t>
	{
		Value(uintmax_t value = 0) : base_t(value) {}
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

	constexpr static size_t encode_read_ut_message_count = 0;
	constexpr static size_t encode_write_ut_message_count = 1;
	constexpr static size_t decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

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
