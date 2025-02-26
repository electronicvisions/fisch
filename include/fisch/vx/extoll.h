#pragma once

#include "fisch/cerealization.h"
#include "fisch/vx/decode.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/extoll.h"
#include "halco/common/geometry.h"
#include "hate/visibility.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct ExtollAddress;
struct ExtollAddressOnExtollNetwork;
} // namespace halco::hicann_dls::vx

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

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
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit Extoll(Value const& value = Value()) : m_value(value) {}

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
	friend std::ostream& operator<<(std::ostream& os, Extoll const& word) SYMBOL_VISIBLE;

	bool operator==(Extoll const& other) const SYMBOL_VISIBLE;
	bool operator!=(Extoll const& other) const SYMBOL_VISIBLE;

	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 2;

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
 * Container for reading and writing an Extoll (quad-)word,
 * i.e. 64-bit to an odfi-rf on an arbitrary network node.
 */
class GENPYBIND(visible) ExtollOnNwNode
{
public:
	typedef halco::hicann_dls::vx::ExtollAddressOnExtollNetwork coordinate_type;

	typedef word_access_type::ExtollOnNwNode Value GENPYBIND(visible);

	/**
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit ExtollOnNwNode(Value const& value = Value()) : m_value(value) {}

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
	friend std::ostream& operator<<(std::ostream& os, ExtollOnNwNode const& word) SYMBOL_VISIBLE;

	bool operator==(ExtollOnNwNode const& other) const SYMBOL_VISIBLE;
	bool operator!=(ExtollOnNwNode const& other) const SYMBOL_VISIBLE;

	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 2;

	static void encode_read(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target)
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace vx
} // namespace fisch

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::Extoll)
FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::ExtollOnNwNode)
