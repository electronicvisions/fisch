#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/reset.h"
#include "hate/visibility.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct ResetChipOnDLS;
} // namespace halco::hicann_dls::vx

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {

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
	explicit ResetChip(Value value = Value(false)) : m_value(value) {}

	/**
	 * Get reset value.
	 * @return Boolean reset value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set reset value.
	 * @param value Boolean value to set reset to
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, ResetChip const& reset) SYMBOL_VISIBLE;

	bool operator==(ResetChip const& other) const SYMBOL_VISIBLE;
	bool operator!=(ResetChip const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace vx
} // namespace fisch::vx

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::ResetChip)
