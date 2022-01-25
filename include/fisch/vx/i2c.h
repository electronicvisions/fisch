#pragma once

#include "fisch/vx/constants.h"
#include "fisch/vx/word_access/type/i2c.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hate/type_index.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct I2CIdRegisterOnBoard;
struct I2CINA219RoRegisterOnBoard;
struct I2CINA219RwRegisterOnBoard;
struct I2CTCA9554RoRegisterOnBoard;
struct I2CTCA9554RwRegisterOnBoard;
struct I2CAD5252RwRegisterOnBoard;
struct I2CDAC6573RwRegisterOnBoard;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

template <typename Derived, typename ValueType, typename CoordinateType>
class I2CRwRegister;

/**
 * General implementation of an I2C read-only register. Its length is determined by the ValueType */
template <typename Derived, typename ValueType, typename CoordinateType>
class I2CRoRegister
{
public:
	typedef CoordinateType coordinate_type;
	typedef ValueType Value;

	static constexpr size_t register_size_bytes =
	    hate::math::round_up_integer_division(hate::math::num_bits(Value::max), 8);

	// Split constructor because of genpybind bug (Issue #3765)
	explicit I2CRoRegister() : m_data(Value()) {}
	explicit I2CRoRegister(I2CRoRegister<Derived, ValueType, CoordinateType>::Value value) :
	    m_data(value)
	{}

	/**
	 * Get value.
	 * @return I2C register value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value I2C register value to set
	 */
	void set(Value value);

	// NOTE: operator needs to be inlined for python wrapping to work
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Derived const& word)
	{
		std::stringstream ss_d;
		// We cast this to an unsigned value to prevent problems with uint8_t values
		ss_d << "0d" << std::dec << static_cast<unsigned>(word.m_data.value());
		std::stringstream ss_x;
		ss_x << "0x" << std::hex << static_cast<unsigned>(word.m_data.value());
		hate::bitset<register_size_bytes * CHAR_BIT> bits(word.m_data.value());
		os << hate::name<Derived>() << "(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits
		   << ")";
		return os;
	}

	bool operator==(Derived const& other) const;
	bool operator!=(Derived const& other) const;


	/* Reading consists of 2 + register_size_bytes messages:
	 * [ addr_write, data_reg, addr_read_0, addr_read_1, ...] */
	static constexpr size_t GENPYBIND(hidden)
	    encode_read_ut_message_count = 2 + register_size_bytes;
	static constexpr size_t GENPYBIND(hidden) encode_write_ut_message_count = 0;
	static constexpr size_t GENPYBIND(hidden) decode_ut_message_count = register_size_bytes;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

protected:
	Value m_data;

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


/**
 * General implementation of an I2C read-write register. Its length is determined by the ValueType.
 */
template <typename Derived, typename ValueType, typename CoordinateType>
class I2CRwRegister : public I2CRoRegister<Derived, ValueType, CoordinateType>
{
public:
	typedef ValueType Value;

	explicit I2CRwRegister() : I2CRoRegister<Derived, ValueType, CoordinateType>() {}
	explicit I2CRwRegister(Value value) : I2CRoRegister<Derived, ValueType, CoordinateType>(value)
	{}

	/* Writing consists of 2 + 2 * register_size_bytes messages:
	 * [ addr_write, data_reg, addr_write, data_0_write, addr_write, data_1_write, ...] */
	static constexpr size_t GENPYBIND(hidden) encode_write_ut_message_count =
	    2 + 2 * I2CRoRegister<Derived, ValueType, CoordinateType>::register_size_bytes;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    CoordinateType const& coord) const GENPYBIND(hidden);
};


typedef word_access_type::I2CIdRegister I2CIdRegisterValue GENPYBIND(visible);

/**
 * Container for reading the unique ID of the chip carrier board's EEPROM.
 */
class GENPYBIND(inline_base("*")) I2CIdRegister
    : public I2CRoRegister<
          I2CIdRegister,
          I2CIdRegisterValue,
          halco::hicann_dls::vx::I2CIdRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CIdRegister() : I2CRoRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CIdRegister(Value value) : I2CRoRegister(value) {}
	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


typedef word_access_type::I2CINA219RoRegister I2CINA219RoRegisterValue GENPYBIND(visible);

/**
 * Container for reading a read-only register of a INA219 voltage/current/power measurement device.
 */
class GENPYBIND(inline_base("*")) I2CINA219RoRegister
    : public I2CRoRegister<
          I2CINA219RoRegister,
          I2CINA219RoRegisterValue,
          halco::hicann_dls::vx::I2CINA219RoRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CINA219RoRegister() : I2CRoRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CINA219RoRegister(Value value) : I2CRoRegister(value) {}
	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


typedef word_access_type::I2CINA219RwRegister I2CINA219RwRegisterValue GENPYBIND(visible);

/**
 * Container for accessing a read-write register of a INA219 voltage/current/power measurement
 * device.
 */
class GENPYBIND(inline_base("*")) I2CINA219RwRegister
    : public I2CRwRegister<
          I2CINA219RwRegister,
          I2CINA219RwRegisterValue,
          halco::hicann_dls::vx::I2CINA219RwRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CINA219RwRegister() : I2CRwRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CINA219RwRegister(Value value) : I2CRwRegister(value) {}
	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


typedef word_access_type::I2CTCA9554RoRegister I2CTCA9554RoRegisterValue GENPYBIND(visible);

/**
 * Container for accessing a read-only I2C register on the TCA9554 IO Expander device.
 */
class GENPYBIND(inline_base("*")) I2CTCA9554RoRegister
    : public I2CRoRegister<
          I2CTCA9554RoRegister,
          I2CTCA9554RoRegisterValue,
          halco::hicann_dls::vx::I2CTCA9554RoRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CTCA9554RoRegister() : I2CRoRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CTCA9554RoRegister(Value value) : I2CRoRegister(value) {}
	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


typedef word_access_type::I2CTCA9554RwRegister I2CTCA9554RwRegisterValue GENPYBIND(visible);

/**
 * Container for accessing a read-write I2C register on the TCA9554 IO Expander device.
 */
class GENPYBIND(inline_base("*")) I2CTCA9554RwRegister
    : public I2CRwRegister<
          I2CTCA9554RwRegister,
          I2CTCA9554RwRegisterValue,
          halco::hicann_dls::vx::I2CTCA9554RwRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CTCA9554RwRegister() : I2CRwRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CTCA9554RwRegister(Value value) : I2CRwRegister(value) {}
	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


typedef word_access_type::I2CAD5252RwRegister I2CAD5252RwRegisterValue GENPYBIND(visible);

/**
 * Container for accessing a read-write register of a AD5252 digital potentiometer channel.
 */
class GENPYBIND(inline_base("*")) I2CAD5252RwRegister
    : public I2CRwRegister<
          I2CAD5252RwRegister,
          I2CAD5252RwRegisterValue,
          halco::hicann_dls::vx::I2CAD5252RwRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CAD5252RwRegister() : I2CRwRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CAD5252RwRegister(Value value) : I2CRwRegister(value) {}
	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};


typedef word_access_type::I2CDAC6573RwRegister I2CDAC6573RwRegisterValue GENPYBIND(visible);

/**
 * Container for accessing a read-write register of a DAC6573
 */
class GENPYBIND(inline_base("*")) I2CDAC6573RwRegister
    : public I2CRwRegister<
          I2CDAC6573RwRegister,
          I2CDAC6573RwRegisterValue,
          halco::hicann_dls::vx::I2CDAC6573RwRegisterOnBoard>
{
public:
	/**
	 * Construct an instance with a default value.
	 */
	explicit I2CDAC6573RwRegister() : I2CRwRegister() {}
	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit I2CDAC6573RwRegister(Value value) : I2CRwRegister(value) {}

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

	static uint8_t GENPYBIND(hidden) get_register_address(coordinate_type const&);
	static halco::hicann_dls::vx::OmnibusAddress GENPYBIND(hidden)
	    get_base_address(coordinate_type const&);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx
