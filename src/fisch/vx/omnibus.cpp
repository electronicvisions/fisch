#include <array>

#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"

#include "hate/bitset.h"

namespace fisch::vx {

Omnibus::Omnibus() : m_data() {}

Omnibus::Omnibus(value_type const value) : m_data(value) {}

Omnibus::value_type Omnibus::get() const
{
	return m_data;
}

void Omnibus::set(value_type const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, Omnibus const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<sizeof(typename Omnibus::value_type::value_type) * CHAR_BIT> bits(
	    word.m_data.value());
	os << "Omnibus(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool Omnibus::operator==(Omnibus const& other) const
{
	return (m_data == other.m_data);
}

bool Omnibus::operator!=(Omnibus const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, Omnibus::encode_read_ut_message_count>
Omnibus::encode_read(coordinate_type const& coord)
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::address;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count> ret;

	ret[0] = hxcomm::vx::ut_message_to_fpga<address>(address::payload_type(coord.value(), true));
	return ret;
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, Omnibus::encode_write_ut_message_count>
Omnibus::encode_write(coordinate_type const& coord) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;

	ret[0] = hxcomm::vx::ut_message_to_fpga<address>(address::payload_type(coord.value(), false));

	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(data::payload_type(m_data.value()));

	return ret;
}

void Omnibus::decode(
    std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const& messages)
{
	using data = hxcomm::vx::instruction::omnibus_from_fpga::data;

	auto data_message = boost::get<hxcomm::vx::ut_message_from_fpga<data> >(messages[0]);
	m_data = value_type(static_cast<uint32_t>(data_message.get_payload()));
}

template <class Archive>
void Omnibus::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Omnibus)

} // namespace fisch::vx
