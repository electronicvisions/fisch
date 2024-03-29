#include "fisch/vx/i2c.h"

#include "fisch/vx/encode.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hate/bitset.h"
#include "hxcomm/vx/utmessage.h"
#include <array>

namespace fisch::vx {

template <typename Derived, typename ValueType, typename CoordinateType>
typename I2CRoRegister<Derived, ValueType, CoordinateType>::Value
I2CRoRegister<Derived, ValueType, CoordinateType>::get() const
{
	return m_value;
}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRoRegister<Derived, ValueType, CoordinateType>::set(Value const value)
{
	m_value = value;
}

template <typename Derived, typename ValueType, typename CoordinateType>
bool I2CRoRegister<Derived, ValueType, CoordinateType>::operator==(Derived const& other) const
{
	return (m_value == other.m_value);
}

template <typename Derived, typename ValueType, typename CoordinateType>
bool I2CRoRegister<Derived, ValueType, CoordinateType>::operator!=(Derived const& other) const
{
	return !(*this == other);
}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRoRegister<Derived, ValueType, CoordinateType>::encode_read(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target)
{
	/* The register- and base addresses depends on the coordinate in a non-trivial way. The concrete
	 * implementation needs to convert this */
	auto register_addr = Derived::get_register_address(coord);
	halco::hicann_dls::vx::OmnibusAddress base_addr = Derived::get_base_address(coord);

	Omnibus(Omnibus::Value(register_addr)).encode_write(base_addr, target);

	// The I2C controller will automatically send a repeat start when the r/w mode changes.
	if (encode_read_ut_message_count > 3) {
		for (size_t i = 2; i < encode_read_ut_message_count - 1; i++) {
			Omnibus::encode_read(base_addr, target);
		}
	}

	Omnibus::encode_read(
	    halco::hicann_dls::vx::OmnibusAddress(base_addr | i2c_over_omnibus_stop), target);
}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRoRegister<Derived, ValueType, CoordinateType>::encode_write(
    coordinate_type const& /*coord*/, UTMessageToFPGABackEmplacer& /*target*/) const
{}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRoRegister<Derived, ValueType, CoordinateType>::decode(
    UTMessageFromFPGARangeOmnibus const& messages)
{
	typename Value::value_type value = 0;
	for (size_t i = 0; i < messages.size(); ++i) {
		Omnibus word;
		word.decode({messages.begin() + i, messages.begin() + i});

		// Check for I2C ack
		if (word.get().word.value() & (1ul << 31)) {
			throw std::runtime_error("I2C ack missing, data invalid.");
		}

		value |= (word.get().word.value() & 0xff) << ((messages.size() - (i + 1)) * CHAR_BIT);
	}

	m_value = Value(value);
}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRwRegister<Derived, ValueType, CoordinateType>::encode_write(
    CoordinateType const& coord, UTMessageToFPGABackEmplacer& target) const
{
	// I2C works on byte-sized chunks of data.
	const int sizeof_data = I2CRoRegister<Derived, ValueType, CoordinateType>::register_size_bytes;
	std::array<uint8_t, sizeof_data> data;
	for (int i = 0; i < sizeof_data; i++) {
		data[i] = 0xFF & (this->m_value >> (sizeof_data - (i + 1)) * CHAR_BIT);
	}

	/* The register- and base addresses depends on the coordinate in a non-trivial way. The concrete
	 * implementation needs to convert this */
	auto register_addr = Derived::get_register_address(coord);
	halco::hicann_dls::vx::OmnibusAddress base_addr = Derived::get_base_address(coord);

	Omnibus(Omnibus::Value(register_addr)).encode_write(base_addr, target);

	if (sizeof_data > 1) {
		for (size_t i = 0; i < sizeof_data - 1; i++) {
			Omnibus(Omnibus::Value(data[i])).encode_write(base_addr, target);
		}
	}

	Omnibus(Omnibus::Value(data[sizeof_data - 1]))
	    .encode_write(
	        halco::hicann_dls::vx::OmnibusAddress(base_addr | i2c_over_omnibus_stop), target);
}

/**
 * NOTE: The I2CRwRegister inherits most of its implementation from the read-only variant. Since
 * explicit instantiation does not care about inherited members, we need to instantiate both the
 * RW and RO base-class. */
#define EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(CLASS_NAME)                                     \
	template class I2CRoRegister<CLASS_NAME, CLASS_NAME::Value, CLASS_NAME::coordinate_type>;
#define EXPLICIT_INSTANTIATE_FISCH_I2C_RW_REGISTER(CLASS_NAME)                                     \
	template class I2CRoRegister<CLASS_NAME, CLASS_NAME::Value, CLASS_NAME::coordinate_type>;      \
	template class I2CRwRegister<CLASS_NAME, CLASS_NAME::Value, CLASS_NAME::coordinate_type>;


uint8_t I2CIdRegister::get_register_address(coordinate_type const& /*coord*/)
{
	// We interface with an EEPROM which stores its 32Bit ID starting at register 0xFC.
	return static_cast<uint8_t>(0xfc);
}

halco::hicann_dls::vx::OmnibusAddress I2CIdRegister::get_base_address(
    coordinate_type const& /*coord*/)
{
	return halco::hicann_dls::vx::OmnibusAddress(i2c_eeprom_base_address);
}


uint8_t I2CTempRegister::get_register_address(coordinate_type const& /*coord*/)
{
	// The read-only temperature is stored at register address 0.
	return static_cast<uint8_t>(0x00);
}

halco::hicann_dls::vx::OmnibusAddress I2CTempRegister::get_base_address(
    coordinate_type const& /*coord*/)
{
	return halco::hicann_dls::vx::OmnibusAddress(i2c_tmp112_base_address);
}


uint8_t I2CINA219RoRegister::get_register_address(coordinate_type const& coord)
{
	// The RO registers start at 1 and are indexed sequentially
	return static_cast<uint8_t>(coord.toI2CINA219RoRegisterOnINA219() + 1);
}

halco::hicann_dls::vx::OmnibusAddress I2CINA219RoRegister::get_base_address(
    coordinate_type const& coord)
{
	return halco::hicann_dls::vx::OmnibusAddress(i2c_ina219_base_address + coord.toINA219OnBoard());
}


uint8_t I2CINA219RwRegister::get_register_address(coordinate_type const& coord)
{
	// The RW registers are at index 0 and 5, the coordinates are sequential.
	constexpr static std::array<uint8_t, 2> ina_regs{0, 5};
	return ina_regs.at(coord.toI2CINA219RwRegisterOnINA219());
}

halco::hicann_dls::vx::OmnibusAddress I2CINA219RwRegister::get_base_address(
    coordinate_type const& coord)
{
	return halco::hicann_dls::vx::OmnibusAddress(i2c_ina219_base_address + coord.toINA219OnBoard());
}


uint8_t I2CTCA9554RoRegister::get_register_address(coordinate_type const& coord)
{
	// The RO register(s) starts at 0 and is the only one.
	return static_cast<uint8_t>(coord.toI2CTCA9554RoRegisterOnTCA9554());
}

halco::hicann_dls::vx::OmnibusAddress I2CTCA9554RoRegister::get_base_address(
    coordinate_type const& coord)
{
	return halco::hicann_dls::vx::OmnibusAddress(
	    i2c_tca9554_base_address + coord.toTCA9554OnBoard());
}


uint8_t I2CTCA9554RwRegister::get_register_address(coordinate_type const& coord)
{
	// The RW registers start at 1 and are indexed sequentially
	return static_cast<uint8_t>(coord.toI2CTCA9554RwRegisterOnTCA9554() + 1);
}

halco::hicann_dls::vx::OmnibusAddress I2CTCA9554RwRegister::get_base_address(
    coordinate_type const& coord)
{
	return halco::hicann_dls::vx::OmnibusAddress(
	    i2c_tca9554_base_address + coord.toTCA9554OnBoard());
}


uint8_t I2CAD5252RwRegister::get_register_address(coordinate_type const& coord)
{
	constexpr static std::array<uint8_t, 2> register_on_channel{
	    halco::hicann_dls::vx::I2CAD5252RwRegisterOnAD5252Channel::rdac_volatile_prefix,
	    halco::hicann_dls::vx::I2CAD5252RwRegisterOnAD5252Channel::eemem_persistent_prefix};
	constexpr static std::array<uint8_t, 2> channel_on_da5252{0x01, 0x03};

	return register_on_channel.at(coord.toI2CAD5252RwRegisterOnAD5252Channel()) +
	       channel_on_da5252.at(coord.toAD5252ChannelOnAD5252());
}

halco::hicann_dls::vx::OmnibusAddress I2CAD5252RwRegister::get_base_address(
    coordinate_type const& coord)
{
	// The AD5252 on the ultra96 have addresses 0x2d, 0x2e, 0x2f in this order. 0x2c is omitted.
	return halco::hicann_dls::vx::OmnibusAddress(
	    i2c_ad5252_base_address + coord.toAD5252ChannelOnBoard().toAD5252OnBoard() + 1);
}


void I2CDAC6573RwRegister::encode_write(
    I2CDAC6573RwRegister::coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
    GENPYBIND(hidden)
{
	// The hw-register only uses the 10 MSB out of 16 bits in the register. We need to shift the
	// data 6 bits to the left to align it properly accross the two I2C bytes.
	const int sizeof_data = I2CDAC6573RwRegister::register_size_bytes;

	std::array<uint8_t, sizeof_data> data;
	uint16_t shifted_data = m_value << 6;

	for (int i = 0; i < sizeof_data; i++) {
		data[i] = 0xFF & (shifted_data >> (sizeof_data - (i + 1)) * CHAR_BIT);
	}

	// The following is simply I2CRwRegister::encode_write in the special case of 2 data-bytes.
	auto register_addr = I2CDAC6573RwRegister::get_register_address(coord);
	halco::hicann_dls::vx::OmnibusAddress base_addr = I2CDAC6573RwRegister::get_base_address(coord);

	Omnibus(Omnibus::Value(register_addr)).encode_write(base_addr, target);

	Omnibus(Omnibus::Value(data[0])).encode_write(base_addr, target);

	Omnibus(Omnibus::Value(data[1]))
	    .encode_write(
	        halco::hicann_dls::vx::OmnibusAddress(base_addr | i2c_over_omnibus_stop), target);
}

void I2CDAC6573RwRegister::decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden)
{
	Value::value_type value = 0;
	for (size_t i = 0; i < messages.size(); ++i) {
		Omnibus word;
		word.decode({messages.begin() + i, messages.begin() + i});

		// Check for I2C ack
		if (word.get().word.value() & (1ul << 31)) {
			throw std::runtime_error("I2C ack missing, data invalid.");
		}

		value |= (word.get().word.value() & 0xff) << ((messages.size() - (i + 1)) * CHAR_BIT);
	}

	// The hw-register only uses the 10 MSB out of 16 bits.
	m_value = Value(value >> 6);
}

uint8_t I2CDAC6573RwRegister::get_register_address(coordinate_type const& coord)
{
	return halco::hicann_dls::vx::I2CDAC6573RwRegisterOnBoard::register_base_addr +
	       (coord.toDAC6573ChannelOnDAC6573() << 1);
}

halco::hicann_dls::vx::OmnibusAddress I2CDAC6573RwRegister::get_base_address(
    coordinate_type const& coord)
{
	return halco::hicann_dls::vx::OmnibusAddress(
	    i2c_dac6573_base_address + coord.toDAC6573OnBoard());
}

EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(I2CIdRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(I2CTempRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(I2CINA219RoRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(I2CTCA9554RoRegister)

EXPLICIT_INSTANTIATE_FISCH_I2C_RW_REGISTER(I2CINA219RwRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RW_REGISTER(I2CTCA9554RwRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RW_REGISTER(I2CAD5252RwRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RW_REGISTER(I2CDAC6573RwRegister)

} // namespace fisch::vx
