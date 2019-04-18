#include <array>

#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/spi.h"

#include "hate/bitset.h"

namespace fisch::vx {

SPIShiftRegister::SPIShiftRegister() : m_data() {}

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

std::array<hxcomm::vx::ut_message_to_fpga_variant, SPIShiftRegister::encode_read_ut_message_count>
SPIShiftRegister::encode_read(coordinate_type const& /*coord*/)
{
	return {};
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, SPIShiftRegister::encode_write_ut_message_count>
SPIShiftRegister::encode_write(coordinate_type const& /*coord*/) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;

	auto addr = hxcomm::vx::ut_message_to_fpga<address>(
	    address::payload_type(spi_over_omnibus_mask | 1, false));

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	ret[0] = addr;
	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(static_cast<uint8_t>(m_data.value() >> CHAR_BIT * 2)));
	ret[2] = addr;
	ret[3] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(static_cast<uint8_t>(m_data.value() >> CHAR_BIT * 1)));
	ret[4] = addr;
	ret[5] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(spi_over_omnibus_stop_bit | static_cast<uint8_t>(m_data.value())));

	return ret;
}

void SPIShiftRegister::decode(std::array<
                              hxcomm::vx::ut_message_from_fpga_variant,
                              decode_ut_message_count> const& /*messages*/)
{}

template <class Archive>
void SPIShiftRegister::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIShiftRegister)


SPIDACDataRegister::SPIDACDataRegister() : m_data() {}

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

std::array<hxcomm::vx::ut_message_to_fpga_variant, SPIDACDataRegister::encode_read_ut_message_count>
SPIDACDataRegister::encode_read(coordinate_type const& /*coord*/)
{
	return {};
}

std::
    array<hxcomm::vx::ut_message_to_fpga_variant, SPIDACDataRegister::encode_write_ut_message_count>
    SPIDACDataRegister::encode_write(coordinate_type const& coord) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;

	auto addr = hxcomm::vx::ut_message_to_fpga<address>(
	    address::payload_type(spi_over_omnibus_mask | (2 + coord.toDACOnBoard().toEnum()), false));

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	ret[0] = addr;
	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(data::payload_type(
	    ((coord.toSPIDACDataRegisterOnDAC().toEnum() << 12) | m_data.value()) >> CHAR_BIT));

	ret[2] = addr;
	ret[3] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(spi_over_omnibus_stop_bit | static_cast<uint8_t>(m_data.value())));

	return ret;
}

void SPIDACDataRegister::decode(std::array<
                                hxcomm::vx::ut_message_from_fpga_variant,
                                decode_ut_message_count> const& /*messages*/)
{}

template <class Archive>
void SPIDACDataRegister::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIDACDataRegister)


SPIDACControlRegister::SPIDACControlRegister() : m_data() {}

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

std::array<
    hxcomm::vx::ut_message_to_fpga_variant,
    SPIDACControlRegister::encode_read_ut_message_count>
SPIDACControlRegister::encode_read(coordinate_type const& /*coord*/)
{
	return {};
}

std::array<
    hxcomm::vx::ut_message_to_fpga_variant,
    SPIDACControlRegister::encode_write_ut_message_count>
SPIDACControlRegister::encode_write(coordinate_type const& coord) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;

	auto addr = hxcomm::vx::ut_message_to_fpga<address>(
	    address::payload_type(spi_over_omnibus_mask | (2 + coord.toDACOnBoard().toEnum()), false));

	constexpr uint32_t control_mask = 1 << 7;

	// The SPI omnibus master accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, which is unique for the SPI client, until the highest bit (stop bit) is
	// set. Then the collected data is communicated to the client.
	ret[0] = addr;
	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(control_mask | (coord.toSPIDACControlRegisterOnDAC().toEnum() << 5)));

	ret[2] = addr;
	ret[3] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(spi_over_omnibus_stop_bit | m_data.value()));

	return ret;
}

void SPIDACControlRegister::decode(std::array<
                                   hxcomm::vx::ut_message_from_fpga_variant,
                                   decode_ut_message_count> const& /*messages*/)
{}

template <class Archive>
void SPIDACControlRegister::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(SPIDACControlRegister)

} // namespace fisch::vx
