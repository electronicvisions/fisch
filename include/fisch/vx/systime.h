#pragma once
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct SystimeSyncOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for syncronization of chip and FPGA systime.
 * After syncronization the FPGA will annotate responses with systime information by sending
 * additional timing messages.
 */
class GENPYBIND(visible) SystimeSync
{
public:
	typedef halco::hicann_dls::vx::SystimeSyncOnFPGA coordinate_type;
	typedef bool Value;

	SystimeSync(Value do_sync = false);

	/**
	 * Get systime sync enable value.
	 * On true, systime sync is triggered, on false only a systime update response is emitted.
	 * @return Boolean value
	 */
	Value get() const;

	/**
	 * Set systime sync enable value.
	 * On true, systime sync is triggered, on false only a systime update response is emitted.
	 * @param value Boolean value
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SystimeSync const& systime_sync);

	bool operator==(SystimeSync const& other) const;
	bool operator!=(SystimeSync const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_do_sync;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/** Systime type for FPGA executor times. */
struct GENPYBIND(inline_base("*")) FPGATime
    : public halco::common::detail::RantWrapper<FPGATime, uint64_t, 0x7ffffffffff, 0>
{
	constexpr explicit FPGATime(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

/** Systime type for chip annotated times. */
struct GENPYBIND(inline_base("*")) ChipTime
    : public halco::common::detail::RantWrapper<ChipTime, uint64_t, 0x7ffffffffff, 0>
{
	constexpr explicit ChipTime(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
	    rant_t(value)
	{}
};

} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::FPGATime)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::ChipTime)

} // namespace std
