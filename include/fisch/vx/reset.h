#pragma once
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/reset.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct ResetChipOnDLS;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for writing the chip reset.
 */
class GENPYBIND(visible) ResetChip
{
public:
	typedef halco::hicann_dls::vx::ResetChipOnDLS coordinate_type;
	typedef word_access_type::ResetChip Value;

	/**
	 * Construct reset with boolean value.
	 * @param value Boolean value to set
	 */
	explicit ResetChip(Value value = Value(false));

	/**
	 * Get reset value.
	 * @return Boolean reset value
	 */
	Value get() const;

	/**
	 * Set reset value.
	 * @param value Boolean value to set reset to
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, ResetChip const& reset);

	bool operator==(ResetChip const& other) const;
	bool operator!=(ResetChip const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx
