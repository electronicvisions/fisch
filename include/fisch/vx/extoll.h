#pragma once

#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/extoll.h"
#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct ExtollAddress;
struct ExtollAddressOnExtollNetwork;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing an Extoll (quad-)word,
 * i.e., 64-bit to the odfi-rf on the own FPGA node.
 */
class GENPYBIND(visible) Extoll
{
public:
	typedef halco::hicann_dls::vx::ExtollAddress coordinate_type;

	typedef word_access_type::Extoll Value GENPYBIND(visible);

	/**
	 * Default constructor.
	 */
	explicit Extoll();

	/**
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit Extoll(Value const& value);

	/**
	 * Get value.
	 * @return Word value
	 */
	Value const& get() const;

	/**
	 * Set value.
	 * @param value Word value to set
	 */
	void set(Value const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Extoll const& word);

	bool operator==(Extoll const& other) const;
	bool operator!=(Extoll const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 2;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 4;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 2;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for reading and writing an Extoll (quad-)word,
 * i.e. 64-bit to an odfi-rf on an arbitrary network node.
 */
class GENPYBIND(visible) ExtollOnNwNode
{
public:
	typedef halco::hicann_dls::vx::ExtollAddressOnExtollNetwork coordinate_type;

	typedef word_access_type::ExtollOnNwNode Value GENPYBIND(visible);

	/**
	 * Default constructor.
	 */
	explicit ExtollOnNwNode();

	/**
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit ExtollOnNwNode(Value const& value);

	/**
	 * Get value.
	 * @return Word value
	 */
	Value const& get() const;

	/**
	 * Set value.
	 * @param value Word value to set
	 */
	void set(Value const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, ExtollOnNwNode const& word);

	bool operator==(ExtollOnNwNode const& other) const;
	bool operator!=(ExtollOnNwNode const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 6;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 8;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 2;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx
