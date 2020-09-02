#pragma once
#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/constants.h"
#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct FIRFilterOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing the config register of a FIR filter.
 */
class GENPYBIND(visible) FIRFilterRegister
{
public:
	typedef halco::hicann_dls::vx::FIRFilterOnFPGA coordinate_type;

	/** FIR filter register value. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint_fast32_t, 0xffffff, 0>
	{
		constexpr explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
		    rant_t(value)
		{}
	};

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	FIRFilterRegister(Value value = Value());

	/**
	 * Get value.
	 * @return FIRFilterRegister value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value FIRFilterRegister word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, FIRFilterRegister const& word);

	bool operator==(FIRFilterRegister const& other) const;
	bool operator!=(FIRFilterRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 26;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::FIRFilterRegister::Value)

} // namespace std
