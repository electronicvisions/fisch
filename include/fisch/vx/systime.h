#pragma once

#include "halco/hicann-dls/vx/coordinates.h"

#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/genpybind.h"

namespace cereal {
class access;
} // namespace cereal

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

	SystimeSync(bool do_sync = false);

	/**
	 * Get systime sync enable value.
	 * On true, systime sync is triggered, on false only a systime update response is emitted.
	 * @return Boolean value
	 */
	bool get() const;

	/**
	 * Set systime sync enable value.
	 * On true, systime sync is triggered, on false only a systime update response is emitted.
	 * @param value Boolean value
	 */
	void set(bool value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SystimeSync const& systime_sync);

	bool operator==(SystimeSync const& other) const;
	bool operator!=(SystimeSync const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(
	    std::array<hxcomm::vx::UTMessageFromFPGAVariant, decode_ut_message_count> const& messages)
	    GENPYBIND(hidden);

private:
	bool m_do_sync;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
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
