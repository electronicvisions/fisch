#include "fisch/vx/omnibus.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hxcomm/vx/utmessage.h"
#include "hate/bitset.h"
#include "hate/join.h"

namespace fisch::vx {

Omnibus::Omnibus() : m_data() {}
Omnibus::Omnibus(Value const& value) : m_data(value) {}

Omnibus::Value const& Omnibus::get() const
{
	return m_data;
}

void Omnibus::set(Value const& value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, Omnibus const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.word.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.word.value();
	hate::bitset<sizeof(typename Omnibus::Value::value_type) * CHAR_BIT> bits(
	    word.m_data.word.value());
	os << "Omnibus(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits
	   << ", byte_enables: " << hate::join_string(word.m_data.byte_enables, "") << ")";
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

	hate::bitset<std::tuple_size<Value::ByteEnables>::value> byte_enables;
	for (size_t i = 0; i < byte_enables.size; ++i) {
		byte_enables.set(i, m_data.byte_enables.at(i));
	}

	ret[0] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(coord.value(), false, byte_enables));

	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(m_data.word.value()));

	return ret;
}

void Omnibus::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	m_data.word = Value::Word(static_cast<uint32_t>(messages[0].decode()));
	m_data.byte_enables = {true, true, true, true};
}

template <class Archive>
void Omnibus::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Omnibus)


PollingOmnibusBlock::PollingOmnibusBlock(Value const value) : m_value(value) {}

std::ostream& operator<<(std::ostream& os, PollingOmnibusBlock const& config)
{
	std::stringstream ss;
	ss << "PollingOmnibusBlock(" << std::boolalpha << config.m_value << ")";
	return (os << ss.str());
}

bool PollingOmnibusBlock::operator==(PollingOmnibusBlock const& other) const
{
	return m_value == other.m_value;
}

bool PollingOmnibusBlock::operator!=(PollingOmnibusBlock const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, PollingOmnibusBlock::encode_write_ut_message_count>
PollingOmnibusBlock::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::PollingOmnibusBlock>(
	    hxcomm::vx::instruction::timing::PollingOmnibusBlock::Payload(m_value))};
}

template <class Archive>
void PollingOmnibusBlock::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(PollingOmnibusBlock)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Omnibus::Value, 0)
CEREAL_CLASS_VERSION(fisch::vx::Omnibus, 1)
CEREAL_CLASS_VERSION(fisch::vx::PollingOmnibusBlock, 0)
