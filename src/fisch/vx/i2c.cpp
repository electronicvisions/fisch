#include "fisch/vx/i2c.h"

#include <array>

#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/i2c.h"

#include "hate/bitset.h"

namespace fisch::vx {

I2CIdRegister::I2CIdRegister(Value const value) : m_data(value) {}

I2CIdRegister::Value I2CIdRegister::get() const
{
	return m_data;
}

void I2CIdRegister::set(Value const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, I2CIdRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<sizeof(uint32_t) * CHAR_BIT> bits(word.m_data.value());
	os << "I2CIdRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool I2CIdRegister::operator==(I2CIdRegister const& other) const
{
	return (m_data == other.m_data);
}

bool I2CIdRegister::operator!=(I2CIdRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, I2CIdRegister::encode_read_ut_message_count>
I2CIdRegister::encode_read(coordinate_type const& /*coord*/)
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	auto base_addr = Omnibus::coordinate_type(i2c_eeprom_base_address);

	// the I2C EEPROM access first writes the ID base address to the EEPROM address pointer
	// register. Then four bytes are read continuously with automatic read pointer incrementation.
	auto encoded1 = Omnibus(Omnibus::Value(static_cast<uint8_t>(0xfc)))
	                    .encode_write(Omnibus::coordinate_type(base_addr));
	ret[0] = encoded1[0];
	ret[1] = encoded1[1];
	auto encoded2 =
	    Omnibus::encode_read(Omnibus::coordinate_type(base_addr | i2c_over_omnibus_repeat_start));
	ret[2] = encoded2[0];
	auto encoded3 = Omnibus::encode_read(Omnibus::coordinate_type(base_addr));
	ret[3] = encoded3[0];
	ret[4] = encoded3[0];
	auto encoded4 =
	    Omnibus::encode_read(Omnibus::coordinate_type(base_addr | i2c_over_omnibus_stop));
	ret[5] = encoded4[0];

	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, I2CIdRegister::encode_write_ut_message_count>
I2CIdRegister::encode_write(coordinate_type const& /*coord*/) const
{
	return {};
}

namespace {

void check_i2c_ack(Omnibus const& word)
{
	if (word.get().value() & (1ul << 31)) {
		throw std::runtime_error("I2C ack missing, data invalid.");
	}
}

} // namespace

void I2CIdRegister::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	uint32_t value = 0;
	for (size_t i = 0; i < messages.size(); ++i) {
		Omnibus word;
		word.decode({messages.begin() + i, messages.begin() + i});
		check_i2c_ack(word);
		value |= (word.get() & 0xff) << ((messages.size() - (i + 1)) * CHAR_BIT);
	}

	m_data = Value(value);
}

template <class Archive>
void I2CIdRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CIdRegister)


I2CINA219RoRegister::I2CINA219RoRegister(Value const value) : m_data(value) {}

I2CINA219RoRegister::Value I2CINA219RoRegister::get() const
{
	return m_data;
}

void I2CINA219RoRegister::set(Value const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, I2CINA219RoRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<sizeof(uint16_t) * CHAR_BIT> bits(word.m_data.value());
	os << "I2CINA219RoRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool I2CINA219RoRegister::operator==(I2CINA219RoRegister const& other) const
{
	return (m_data == other.m_data);
}

bool I2CINA219RoRegister::operator!=(I2CINA219RoRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, I2CINA219RoRegister::encode_read_ut_message_count>
I2CINA219RoRegister::encode_read(coordinate_type const& coord)
{
	// read-only registers start at register number 1 and are then sequentially indexed
	uint32_t ina_reg = coord.toI2CINA219RoRegisterOnINA219() + 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	auto base_addr = Omnibus::coordinate_type(i2c_ina219_base_address + coord.toINA219OnBoard());

	// the I2C INA219 access first writes the register address.
	// Then two bytes are read continuously with automatic read pointer incrementation.
	auto encoded1 = Omnibus(Omnibus::Value(ina_reg)).encode_write(base_addr);
	ret[0] = encoded1[0];
	ret[1] = encoded1[1];
	auto encoded2 = Omnibus::encode_read(Omnibus::coordinate_type(base_addr));
	ret[2] = encoded2[0];
	auto encoded3 =
	    Omnibus::encode_read(Omnibus::coordinate_type(base_addr | i2c_over_omnibus_stop));
	ret[3] = encoded3[0];

	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, I2CINA219RoRegister::encode_write_ut_message_count>
I2CINA219RoRegister::encode_write(coordinate_type const& /*coord*/) const
{
	return {};
}

void I2CINA219RoRegister::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	uint32_t value = 0;
	for (size_t i = 0; i < messages.size(); ++i) {
		Omnibus word;
		word.decode({messages.begin() + i, messages.begin() + i});
		check_i2c_ack(word);
		value |= (word.get() & 0xff) << ((messages.size() - (i + 1)) * CHAR_BIT);
	}

	m_data = Value(value);
}

template <class Archive>
void I2CINA219RoRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CINA219RoRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::I2CIdRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CINA219RoRegister, 0)
