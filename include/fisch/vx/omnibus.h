#pragma once

#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/omnibus_data.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct OmnibusAddress;
struct PollingOmnibusBlockOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing an omnibus word.
 */
class GENPYBIND(visible) Omnibus
{
public:
	typedef halco::hicann_dls::vx::OmnibusAddress coordinate_type;
	typedef OmnibusData Value GENPYBIND(opaque(false));
	typedef std::array<bool, sizeof(OmnibusData::value_type)> ByteEnables;

	/**
	 * Default constructor.
	 */
	explicit Omnibus();

	/**
	 * Construct an instance with a word value.
	 * @param value Word value to construct instance with
	 */
	explicit Omnibus(Value value);

	/**
	 * Construct an instance with a word value and byte enables.
	 * @param value Word value to construct instance with
	 * @param byte_enables Byte enables to construct instance with
	 */
	explicit Omnibus(Value value, ByteEnables byte_enables);

	/**
	 * Get value.
	 * @return Word value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value Word value to set
	 */
	void set(Value value);

	/**
	 * Get byte enables.
	 * @return ByteEnables value
	 */
	ByteEnables const& get_byte_enables() const;

	/**
	 * Set byte enables.
	 * @param value ByteEnables value
	 */
	void set_byte_enables(ByteEnables const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Omnibus const& word);

	bool operator==(Omnibus const& other) const;
	bool operator!=(Omnibus const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 2;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

private:
	Value m_data;
	ByteEnables m_byte_enables;

	friend class cereal::access;
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
	typedef bool Value;

	/**
	 * Resolve block if value is true (*address & mask == target) or if value is false (*address &
	 * mask != target).
	 */
	PollingOmnibusBlock(Value value = true);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PollingOmnibusBlock const& config);

	bool operator==(PollingOmnibusBlock const& other) const;
	bool operator!=(PollingOmnibusBlock const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);

	Value m_value;
};

} // namespace fisch::vx
