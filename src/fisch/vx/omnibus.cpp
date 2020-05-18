#include "fisch/vx/omnibus.h"

#include <array>
#include <cereal/types/array.hpp>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hxcomm/vx/utmessage.h"
#include "hate/bitset.h"
#include "hate/join.h"

namespace fisch::vx {

Omnibus::Omnibus() : m_data(), m_byte_enables({true, true, true, true}) {}
Omnibus::Omnibus(Value const value) : m_data(value), m_byte_enables({true, true, true, true}) {}
Omnibus::Omnibus(Value const value, ByteEnables const byte_enables) :
    m_data(value), m_byte_enables(byte_enables)
{}

Omnibus::Value Omnibus::get() const
{
	return m_data;
}

void Omnibus::set(Value const value)
{
	m_data = value;
}

Omnibus::ByteEnables const& Omnibus::get_byte_enables() const
{
	return m_byte_enables;
}

void Omnibus::set_byte_enables(ByteEnables const& value)
{
	m_byte_enables = value;
}

std::ostream& operator<<(std::ostream& os, Omnibus const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<sizeof(typename Omnibus::Value::value_type) * CHAR_BIT> bits(word.m_data.value());
	os << "Omnibus(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits
	   << ", byte_enables: " << hate::join_string(word.m_byte_enables, "") << ")";
	return os;
}

bool Omnibus::operator==(Omnibus const& other) const
{
	return (m_data == other.m_data) && (m_byte_enables == other.m_byte_enables);
}

bool Omnibus::operator!=(Omnibus const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Omnibus::encode_read_ut_message_count>
Omnibus::encode_read(coordinate_type const& coord)
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	ret[0] = hxcomm::vx::UTMessageToFPGA<address>(address::Payload(coord.value(), true));
	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Omnibus::encode_write_ut_message_count>
Omnibus::encode_write(coordinate_type const& coord) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::Data;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	hate::bitset<std::tuple_size<ByteEnables>::value> byte_enables;
	for (size_t i = 0; i < byte_enables.size; ++i) {
		byte_enables.set(i, m_byte_enables.at(i));
	}

	ret[0] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(coord.value(), false, byte_enables));

	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(m_data.value()));

	return ret;
}

void Omnibus::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	m_data = Value(static_cast<uint32_t>(messages[0].decode()));
	m_byte_enables = {true, true, true, true};
}

template <class Archive>
void Omnibus::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
	ar(CEREAL_NVP(m_byte_enables));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Omnibus)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Omnibus, 0)
