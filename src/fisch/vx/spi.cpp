#include "fisch/vx/spi.h"

#include <array>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/spi.h"
#include "hate/bitset.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

SPIShiftRegister::SPIShiftRegister(Value const value) : m_data(value) {}

SPIShiftRegister::Value SPIShiftRegister::get() const
{
	return m_data;
}

void SPIShiftRegister::set(Value const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, SPIShiftRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<24> bits(word.m_data.value());
	os << "SPIShiftRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool SPIShiftRegister::operator==(SPIShiftRegister const& other) const
{
	return (m_data == other.m_data);
}

bool SPIShiftRegister::operator!=(SPIShiftRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, SPIShiftRegister::encode_write_ut_message_count>
SPIShiftRegister::encode_write(coordinate_type const& /*coord*/) const
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	auto addr = OmnibusFPGA::coordinate_type(spi_over_omnibus_mask | 1);

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	auto encoded1 =
	    OmnibusFPGA(OmnibusFPGA::Value(static_cast<uint8_t>(m_data.value() >> CHAR_BIT * 2)))
	        .encode_write(addr);
	ret[0] = encoded1[0];
	ret[1] = encoded1[1];
	auto encoded2 =
	    OmnibusFPGA(OmnibusFPGA::Value(static_cast<uint8_t>(m_data.value() >> CHAR_BIT * 1)))
	        .encode_write(addr);
	ret[2] = encoded2[0];
	ret[3] = encoded2[1];
	auto encoded3 =
	    OmnibusFPGA(
	        OmnibusFPGA::Value(spi_over_omnibus_stop_bit | static_cast<uint8_t>(m_data.value())))
	        .encode_write(addr);
	ret[4] = encoded3[0];
	ret[5] = encoded3[1];

	return ret;
}

template <class Archive>
void SPIShiftRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIShiftRegister)


SPIDACDataRegister::SPIDACDataRegister(Value const value) : m_data(value) {}

SPIDACDataRegister::Value SPIDACDataRegister::get() const
{
	return m_data;
}

void SPIDACDataRegister::set(Value const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, SPIDACDataRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<12> bits(word.m_data.value());
	os << "SPIDACDataRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool SPIDACDataRegister::operator==(SPIDACDataRegister const& other) const
{
	return (m_data == other.m_data);
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, SPIDACDataRegister::encode_write_ut_message_count>
SPIDACDataRegister::encode_write(coordinate_type const& coord) const
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	auto addr = OmnibusFPGA::coordinate_type(
	    spi_over_omnibus_mask | (2 + (2 * coord.toDACOnBoard().toEnum())));

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	SPIDACDataRegisterBitfield bitfield;
	bitfield.u.m.read = false;
	bitfield.u.m.channel = coord.toSPIDACDataRegisterOnDAC().toEnum();
	bitfield.u.m.data = m_data.value();

	auto encoded1 = OmnibusFPGA(OmnibusFPGA::Value(bitfield.u.raw[1])).encode_write(addr);
	ret[0] = encoded1[0];
	ret[1] = encoded1[1];
	auto encoded2 = OmnibusFPGA(OmnibusFPGA::Value(spi_over_omnibus_stop_bit | bitfield.u.raw[0]))
	                    .encode_write(addr);
	ret[2] = encoded2[0];
	ret[3] = encoded2[1];

	// Write single update LDAC value.
	SPIDACControlRegister update_control(SPIDACControlRegister::Value(0x2));
	auto update_control_messages =
	    update_control.encode_write(halco::hicann_dls::vx::SPIDACControlRegisterOnBoard(
	        halco::hicann_dls::vx::SPIDACControlRegisterOnDAC::ldac, coord.toDACOnBoard()));
	static_assert(
	    update_control_messages.size() == 4,
	    "SPIDACControlRegister does not need 4 messages to be written.");
	ret[4] = update_control_messages[0];
	ret[5] = update_control_messages[1];
	ret[6] = update_control_messages[2];
	ret[7] = update_control_messages[3];
	return ret;
}

template <class Archive>
void SPIDACDataRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIDACDataRegister)


SPIDACControlRegister::SPIDACControlRegister(Value const value) : m_data(value) {}

SPIDACControlRegister::Value SPIDACControlRegister::get() const
{
	return m_data;
}

void SPIDACControlRegister::set(Value const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, SPIDACControlRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<13> bits(word.m_data.value());
	os << "SPIDACControlRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool SPIDACControlRegister::operator==(SPIDACControlRegister const& other) const
{
	return (m_data == other.m_data);
}

bool SPIDACControlRegister::operator!=(SPIDACControlRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, SPIDACControlRegister::encode_write_ut_message_count>
SPIDACControlRegister::encode_write(coordinate_type const& coord) const
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	auto addr = OmnibusFPGA::coordinate_type(
	    spi_over_omnibus_mask | (2 + (2 * coord.toDACOnBoard().toEnum())));

	constexpr uint32_t control_mask = 1 << 7;

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	auto encoded1 =
	    OmnibusFPGA(OmnibusFPGA::Value(
	                    (control_mask | (coord.toSPIDACControlRegisterOnDAC().toEnum() << 5))))
	        .encode_write(addr);
	ret[0] = encoded1[0];
	ret[1] = encoded1[1];

	auto encoded2 = OmnibusFPGA(OmnibusFPGA::Value((spi_over_omnibus_stop_bit | m_data.value())))
	                    .encode_write(addr);
	ret[2] = encoded2[0];
	ret[3] = encoded2[1];

	return ret;
}

template <class Archive>
void SPIDACControlRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIDACControlRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::SPIShiftRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::SPIDACDataRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::SPIDACControlRegister, 0)
