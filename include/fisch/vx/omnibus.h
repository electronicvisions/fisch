#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/decode.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/omnibus.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct OmnibusAddress;
struct PollingOmnibusBlockOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing an omnibus word.
 */
class GENPYBIND(visible) Omnibus
{
public:
	typedef halco::hicann_dls::vx::OmnibusAddress coordinate_type;

	typedef word_access_type::Omnibus Value GENPYBIND(visible);

	/**
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit Omnibus(Value const& value = Value()) : m_value(value) {}

	/**
	 * Get value.
	 * @return Word value
	 */
	Value const& get() const SYMBOL_VISIBLE;

	/**
	 * Set value.
	 * @param value Word value to set
	 */
	void set(Value const& value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Omnibus const& word) SYMBOL_VISIBLE;

	bool operator==(Omnibus const& other) const SYMBOL_VISIBLE;
	bool operator!=(Omnibus const& other) const SYMBOL_VISIBLE;

	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static void encode_read(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target)
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for polling block operation on a Omnibus address.
 * Compare given value from address in FPGA register file masked with expected target result.
 */
class GENPYBIND(visible) PollingOmnibusBlock
{
public:
	typedef halco::hicann_dls::vx::PollingOmnibusBlockOnFPGA coordinate_type;
	typedef word_access_type::PollingOmnibusBlock Value;

	/**
	 * Resolve block if value is true (*address & mask == target) or if value is false (*address &
	 * mask != target).
	 */
	PollingOmnibusBlock(Value value = Value(true)) : m_value(value) {}

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PollingOmnibusBlock const& config)
	    SYMBOL_VISIBLE;

	/**
	 * Get value.
	 * @return Value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set value.
	 * @param value Value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	bool operator==(PollingOmnibusBlock const& other) const SYMBOL_VISIBLE;
	bool operator!=(PollingOmnibusBlock const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);

	Value m_value;
};

} // namespace vx
} // namespace fisch

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::Omnibus)
FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::PollingOmnibusBlock)
