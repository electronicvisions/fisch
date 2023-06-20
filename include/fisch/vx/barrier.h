#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/barrier.h"
#include "hate/visibility.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct BarrierOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for sync operation.
 */
class GENPYBIND(visible) Barrier
{
public:
	typedef halco::hicann_dls::vx::BarrierOnFPGA coordinate_type;

	typedef word_access_type::Barrier Value GENPYBIND(visible);

	explicit Barrier(Value const value = Value()) : m_value(value) {}

	/**
	 * Get value.
	 * @return Barrier value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set value.
	 * @param value Barrier value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Barrier const& config) SYMBOL_VISIBLE;

	bool operator==(Barrier const& other) const SYMBOL_VISIBLE;
	bool operator!=(Barrier const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::Barrier)
