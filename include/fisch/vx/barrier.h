#pragma once
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/barrier.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
class access;
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

	explicit Barrier(Value value = Value());

	/**
	 * Get value.
	 * @return Barrier value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value Barrier value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Barrier const& config);

	bool operator==(Barrier const& other) const;
	bool operator!=(Barrier const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx
