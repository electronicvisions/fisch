#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/systime.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
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
	typedef word_access_type::SystimeSync Value;

	explicit SystimeSync(Value value = Value(false)) : m_value(value) {}

	/**
	 * Get systime sync enable value.
	 * On true, systime sync is triggered, on false only a systime update response is emitted.
	 * @return Boolean value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set systime sync enable value.
	 * On true, systime sync is triggered, on false only a systime update response is emitted.
	 * @param value Boolean value
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SystimeSync const& systime_sync)
	    SYMBOL_VISIBLE;

	bool operator==(SystimeSync const& other) const SYMBOL_VISIBLE;
	bool operator!=(SystimeSync const& other) const SYMBOL_VISIBLE;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::SystimeSync)
