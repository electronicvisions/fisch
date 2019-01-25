#pragma once

#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/omnibus.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx {

/**
 * Container for reading and writing an omnibus word over JTAG.
 */
class OmnibusOnChipOverJTAG
{
public:
	typedef halco::hicann_dls::vx::OmnibusAddress coordinate_type;
	typedef OmnibusData value_type;

	/** Default constructor. */
	OmnibusOnChipOverJTAG();

	/**
	 * Construct an instance with a word value.
	 * @param value Omnibus word value to construct instance with
	 */
	OmnibusOnChipOverJTAG(value_type const& value);

	/**
	 * Get value.
	 * @return Omnibus word value
	 */
	value_type get() const;

	/**
	 * Set value.
	 * @param value Omnibus word value to set
	 */
	void set(value_type const& value);

	bool operator==(OmnibusOnChipOverJTAG const& other) const;
	bool operator!=(OmnibusOnChipOverJTAG const& other) const;

	constexpr static size_t encode_read_ut_message_count = 6;
	constexpr static size_t encode_write_ut_message_count = 6;
	constexpr static size_t decode_ut_message_count = 1;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

private:
	value_type m_data;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};

} // namespace fisch::vx
