#include "fisch/vx/i2c.h"

#include <array>

#include "halco/hicann-dls/vx/omnibus.h"
#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/i2c.h"

#include "hate/bitset.h"

namespace fisch::vx {

template <typename Derived, typename ValueType, typename CoordinateType>
typename I2CRoRegister<Derived, ValueType, CoordinateType>::Value
I2CRoRegister<Derived, ValueType, CoordinateType>::get() const
{
	return m_data;
}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRoRegister<Derived, ValueType, CoordinateType>::set(Value const value)
{
	m_data = value;
}

template <typename Derived, typename ValueType, typename CoordinateType>
bool I2CRoRegister<Derived, ValueType, CoordinateType>::operator==(Derived const& other) const
{
	return (m_data == other.m_data);
}

template <typename Derived, typename ValueType, typename CoordinateType>
bool I2CRoRegister<Derived, ValueType, CoordinateType>::operator!=(Derived const& other) const
{
	return !(*this == other);
}

template <typename Derived, typename ValueType, typename CoordinateType>
std::array<
    hxcomm::vx::UTMessageToFPGAVariant,
    I2CRoRegister<Derived, ValueType, CoordinateType>::encode_read_ut_message_count>
I2CRoRegister<Derived, ValueType, CoordinateType>::encode_read(coordinate_type const& coord)
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	/* The register- and base addresses depends on the coordinate in a non-trivial way. The concrete
	 * implementation needs to convert this */
	auto register_addr = Derived::get_register_address(coord);
	halco::hicann_dls::vx::OmnibusAddress base_addr = Derived::get_base_address(coord);

	auto enc_write_register = Omnibus(Omnibus::Value(register_addr)).encode_write(base_addr);
	ret[0] = enc_write_register[0];
	ret[1] = enc_write_register[1];

	// The I2C controller will automatically send a repeat start when the r/w mode changes.
	auto enc_read = Omnibus::encode_read(base_addr);
	if (encode_read_ut_message_count > 3) {
		for (size_t i = 2; i < encode_read_ut_message_count - 1; i++) {
			ret[i] = enc_read[0];
		}
	}

	auto enc_read_stop = Omnibus::encode_read(
	    halco::hicann_dls::vx::OmnibusAddress(base_addr | i2c_over_omnibus_stop));
	ret[encode_read_ut_message_count - 1] = enc_read_stop[0];

	return ret;
}

template <typename Derived, typename ValueType, typename CoordinateType>
std::array<
    hxcomm::vx::UTMessageToFPGAVariant,
    I2CRoRegister<Derived, ValueType, CoordinateType>::encode_write_ut_message_count>
I2CRoRegister<Derived, ValueType, CoordinateType>::encode_write(
    coordinate_type const& /*coord*/) const
{
	return {};
}

template <typename Derived, typename ValueType, typename CoordinateType>
void I2CRoRegister<Derived, ValueType, CoordinateType>::decode(
    UTMessageFromFPGARangeOmnibus const& messages)
{
	typename Value::value_type value = 0;
	for (size_t i = 0; i < messages.size(); ++i) {
		Omnibus word;
		word.decode({messages.begin() + i, messages.begin() + i});

		// Check for I2C ack
		if (word.get().value() & (1ul << 31)) {
			throw std::runtime_error("I2C ack missing, data invalid.");
		}

		value |= (word.get() & 0xff) << ((messages.size() - (i + 1)) * CHAR_BIT);
	}

	m_data = Value(value);
}

template <typename Derived, typename ValueType, typename CoordinateType>
template <class Archive>
void I2CRoRegister<Derived, ValueType, CoordinateType>::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}


template <typename Derived, typename ValueType, typename CoordinateType>
std::array<
    hxcomm::vx::UTMessageToFPGAVariant,
    I2CRwRegister<Derived, ValueType, CoordinateType>::encode_write_ut_message_count>
I2CRwRegister<Derived, ValueType, CoordinateType>::encode_write(CoordinateType const& coord) const
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	// I2C works on byte-sized chunks of data.
	const int sizeof_data = I2CRoRegister<Derived, ValueType, CoordinateType>::register_size_bytes;
	std::array<uint8_t, sizeof_data> data;
	for (int i = 0; i < sizeof_data; i++) {
		data[i] = 0xFF & (this->m_data >> (sizeof_data - (i + 1)) * CHAR_BIT);
	}

	/* The register- and base addresses depends on the coordinate in a non-trivial way. The concrete
	 * implementation needs to convert this */
	auto register_addr = Derived::get_register_address(coord);
	halco::hicann_dls::vx::OmnibusAddress base_addr = Derived::get_base_address(coord);

	auto enc_write_register = Omnibus(Omnibus::Value(register_addr)).encode_write(base_addr);
	ret[0] = enc_write_register[0];
	ret[1] = enc_write_register[1];

	if (sizeof_data > 1) {
		for (size_t i = 0; i < sizeof_data - 1; i++) {
			auto enc_write_data = Omnibus(Omnibus::Value(data[i])).encode_write(base_addr);
			ret[2 + 2 * i] = enc_write_data[0];
			ret[3 + 2 * i] = enc_write_data[1];
		}
	}

	auto enc_write_stop =
	    Omnibus(Omnibus::Value(data[sizeof_data - 1]))
	        .encode_write(halco::hicann_dls::vx::OmnibusAddress(base_addr | i2c_over_omnibus_stop));
	ret[encode_write_ut_message_count - 2] = enc_write_stop[0];
	ret[encode_write_ut_message_count - 1] = enc_write_stop[1];

	return ret;
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

template <class Archive>
void I2CIdRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CIdRegister, Value, coordinate_type>>(this));
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

template <class Archive>
void I2CINA219RoRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CINA219RoRegister, Value, coordinate_type>>(this));
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

template <class Archive>
void I2CINA219RwRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(CEREAL_NVP(m_data));
}


EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(I2CIdRegister)
EXPLICIT_INSTANTIATE_FISCH_I2C_RO_REGISTER(I2CINA219RoRegister)

EXPLICIT_INSTANTIATE_FISCH_I2C_RW_REGISTER(I2CINA219RwRegister)

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CIdRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CINA219RoRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CINA219RwRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::I2CIdRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CINA219RoRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CINA219RwRegister, 0)
