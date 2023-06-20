#include "fisch/vx/spi.h"

#include "fisch/vx/encode.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/spi.h"
#include "hate/bitset.h"
#include "hxcomm/vx/utmessage.h"
#include <array>

namespace fisch::vx {

SPIShiftRegister::Value SPIShiftRegister::get() const
{
	return m_value;
}

void SPIShiftRegister::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, SPIShiftRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_value.value();
	hate::bitset<24> bits(word.m_value.value());
	os << "SPIShiftRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool SPIShiftRegister::operator==(SPIShiftRegister const& other) const
{
	return (m_value == other.m_value);
}

bool SPIShiftRegister::operator!=(SPIShiftRegister const& other) const
{
	return !(*this == other);
}

void SPIShiftRegister::encode_write(
    coordinate_type const& /*coord*/, UTMessageToFPGABackEmplacer& target) const
{
	auto addr = Omnibus::coordinate_type(spi_over_omnibus_mask | 1);

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	Omnibus(Omnibus::Value(static_cast<uint8_t>(m_value.value() >> CHAR_BIT * 2)))
	    .encode_write(addr, target);
	Omnibus(Omnibus::Value(static_cast<uint8_t>(m_value.value() >> CHAR_BIT * 1)))
	    .encode_write(addr, target);
	Omnibus(Omnibus::Value(spi_over_omnibus_stop_bit | static_cast<uint8_t>(m_value.value())))
	    .encode_write(addr, target);
}


SPIDACDataRegister::Value SPIDACDataRegister::get() const
{
	return m_value;
}

void SPIDACDataRegister::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, SPIDACDataRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_value.value();
	hate::bitset<12> bits(word.m_value.value());
	os << "SPIDACDataRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool SPIDACDataRegister::operator==(SPIDACDataRegister const& other) const
{
	return (m_value == other.m_value);
}

bool SPIDACDataRegister::operator!=(SPIDACDataRegister const& other) const
{
	return !(*this == other);
}

namespace {

struct SPIDACDataRegisterBitfield
{
	union
	{
		std::array<uint8_t, 2> raw;
		// clang-format off
		struct __attribute__((packed)) {
			uint16_t data    : 12;
			uint16_t channel :  3;
			uint16_t read    :  1;
		} m;
		// clang-format on
		static_assert(sizeof(raw) == sizeof(m), "sizes of union types should match");
	} u;

	SPIDACDataRegisterBitfield() : u({{0u, 0u}}) {}

	SPIDACDataRegisterBitfield(std::array<uint8_t, 2> data) : u({data}) {}
};

} // namespace

void SPIDACDataRegister::encode_write(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
{
	auto addr =
	    Omnibus::coordinate_type(spi_over_omnibus_mask | (2 + (2 * coord.toDACOnBoard().toEnum())));

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	SPIDACDataRegisterBitfield bitfield;
	bitfield.u.m.read = false;
	bitfield.u.m.channel = coord.toSPIDACDataRegisterOnDAC().toEnum();
	bitfield.u.m.data = m_value.value();

	Omnibus(Omnibus::Value(bitfield.u.raw[1])).encode_write(addr, target);
	Omnibus(Omnibus::Value(spi_over_omnibus_stop_bit | bitfield.u.raw[0]))
	    .encode_write(addr, target);

	// Write single update LDAC value.
	SPIDACControlRegister update_control(SPIDACControlRegister::Value(0x2));
	update_control.encode_write(
	    halco::hicann_dls::vx::SPIDACControlRegisterOnBoard(
	        halco::hicann_dls::vx::SPIDACControlRegisterOnDAC::ldac, coord.toDACOnBoard()),
	    target);
}


SPIDACControlRegister::Value SPIDACControlRegister::get() const
{
	return m_value;
}

void SPIDACControlRegister::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, SPIDACControlRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_value.value();
	hate::bitset<13> bits(word.m_value.value());
	os << "SPIDACControlRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool SPIDACControlRegister::operator==(SPIDACControlRegister const& other) const
{
	return (m_value == other.m_value);
}

bool SPIDACControlRegister::operator!=(SPIDACControlRegister const& other) const
{
	return !(*this == other);
}

void SPIDACControlRegister::encode_write(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
{
	auto addr =
	    Omnibus::coordinate_type(spi_over_omnibus_mask | (2 + (2 * coord.toDACOnBoard().toEnum())));

	constexpr uint32_t control_mask = 1 << 7;

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	Omnibus(Omnibus::Value((control_mask | (coord.toSPIDACControlRegisterOnDAC().toEnum() << 5))))
	    .encode_write(addr, target);

	Omnibus(Omnibus::Value((spi_over_omnibus_stop_bit | m_value.value())))
	    .encode_write(addr, target);
}

} // namespace fisch::vx
