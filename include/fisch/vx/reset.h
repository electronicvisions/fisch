#pragma once

#include "halco/common/relations.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "hxcomm/vx/utmessage.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx {

/**
 * Container for writing the chip reset.
 */
class ResetChip
{
public:
	typedef halco::hicann_dls::vx::ResetChipOnDLS coordinate_type;

	/** Default constructor. */
	ResetChip();

	/**
	 * Construct reset with boolean value.
	 * @param value Boolean value to set
	 */
	ResetChip(bool value);

	/**
	 * Set reset value
	 * @param value Boolean value to set reset to
	 */
	void set(bool const value);

	bool operator==(ResetChip const& other) const;
	bool operator!=(ResetChip const& other) const;

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
	bool m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};

} // namespace fisch::vx
