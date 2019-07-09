#pragma once

#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct I2CIdRegisterOnBoard;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading the unique ID of the chip carrier board's EEPROM.
 */
class GENPYBIND(visible) I2CIdRegister
{
public:
	typedef halco::hicann_dls::vx::I2CIdRegisterOnBoard coordinate_type;

	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::BaseType<Value, uint32_t>
	{
		explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) : base_t(value) {}
	};

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	I2CIdRegister(Value value = Value());

	/**
	 * Get value.
	 * @return I2CIdRegister value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value I2CIdRegister value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, I2CIdRegister const& word);

	bool operator==(I2CIdRegister const& other) const;
	bool operator!=(I2CIdRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 6;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 4;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::I2CIdRegister::Value)

} // namespace std
