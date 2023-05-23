#include "fisch/vx/extoll.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/extoll.h"
#include "hate/bitset.h"
#include "hate/join.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

namespace {
std::array<halco::hicann_dls::vx::OmnibusAddress, 2> ExtollAddressToOmnibusAddress(
    halco::hicann_dls::vx::ExtollAddress const eaddr)
{
	auto const to_omnibus = [eaddr](size_t const offset) {
		auto omniAddr = halco::hicann_dls::vx::OmnibusAddress((eaddr.value() >> 2) + offset);
		if (omniAddr >= (odfi_rf_omnibus_bit_switch_n - 0x1 + offset)) {
			throw std::runtime_error(
			    "The given Extoll Registerfile Address (" + std::to_string(eaddr.value()) +
			    ") is too large for conversion into the current Omnibus Tree!");
		}
		return halco::hicann_dls::vx::OmnibusAddress(external_omnibus_base_address + omniAddr);
	};

	return {to_omnibus(0x0), to_omnibus(0x1)};
}
} // namespace

Extoll::Value const& Extoll::get() const
{
	return m_value;
}

void Extoll::set(Value const& value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, Extoll const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_value.value();
	hate::bitset<sizeof(typename Extoll::Value::value_type) * CHAR_BIT> bits(word.m_value.value());
	os << "Extoll(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool Extoll::operator==(Extoll const& other) const
{
	return (m_value == other.m_value);
}

bool Extoll::operator!=(Extoll const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Extoll::encode_read_ut_message_count>
Extoll::encode_read(coordinate_type const& coord)
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;

	auto omni_addresses = ExtollAddressToOmnibusAddress(coord);

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	ret[0] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[0].value(), true));
	ret[1] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[1].value(), true));
	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, Extoll::encode_write_ut_message_count>
Extoll::encode_write(coordinate_type const& coord) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::Data;

	auto omni_addresses = ExtollAddressToOmnibusAddress(coord);

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	ret[0] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[0].value(), false));
	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(m_value.value() & 0xffff'ffff));
	ret[2] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[1].value(), false));
	ret[3] =
	    hxcomm::vx::UTMessageToFPGA<data>(data::Payload((m_value.value() >> 32) & 0xffff'ffff));

	return ret;
}

void Extoll::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	m_value = Value(
	    (static_cast<uint64_t>(messages[1].decode()) << 32) |
	    static_cast<uint64_t>(messages[0].decode()));
}

template <class Archive>
void Extoll::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Extoll)


ExtollOnNwNode::Value const& ExtollOnNwNode::get() const
{
	return m_value;
}

void ExtollOnNwNode::set(Value const& value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, ExtollOnNwNode const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_value.value();
	hate::bitset<sizeof(typename ExtollOnNwNode::Value::value_type) * CHAR_BIT> bits(
	    word.m_value.value());
	os << "ExtollOnNwNode(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool ExtollOnNwNode::operator==(ExtollOnNwNode const& other) const
{
	return (m_value == other.m_value);
}

bool ExtollOnNwNode::operator!=(ExtollOnNwNode const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, ExtollOnNwNode::encode_read_ut_message_count>
ExtollOnNwNode::encode_read(coordinate_type const& coord)
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::Data;

	auto omni_addresses = ExtollAddressToOmnibusAddress(coord.toExtollAddress());
	auto coord_ndid_on_nw = coord.toExtollNodeIdOnExtollNetwork();
	auto coord_ndid = coord_ndid_on_nw.toExtollNodeId();
	uint32_t ndid = coord_ndid.value() & 0xffff;
	bool is_self = (coord_ndid == halco::hicann_dls::vx::ExtollNodeIdOnExtollNetwork::self);

	// encode valid-bit together with ndid if it's not a local access:
	uint32_t conf = is_self ? 0x0 : static_cast<uint32_t>(ndid | (0x1 << 16));

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	ret[0] = hxcomm::vx::UTMessageToFPGA<address>(
	    address::Payload(odfi_external_access_config_reg, false));
	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(conf));
	ret[2] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[0].value(), true));
	ret[3] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[1].value(), true));
	ret[4] = hxcomm::vx::UTMessageToFPGA<address>(
	    address::Payload(odfi_external_access_config_reg, false));
	ret[5] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(0x0));
	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, ExtollOnNwNode::encode_write_ut_message_count>
ExtollOnNwNode::encode_write(coordinate_type const& coord) const
{
	using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;
	using data = hxcomm::vx::instruction::omnibus_to_fpga::Data;

	auto omni_addresses = ExtollAddressToOmnibusAddress(coord.toExtollAddress());
	auto coord_ndid = coord.toExtollNodeIdOnExtollNetwork();
	uint16_t ndid = coord_ndid.value() & 0xffff;
	bool is_self = (coord_ndid == halco::hicann_dls::vx::ExtollNodeIdOnExtollNetwork::self);

	uint32_t conf = is_self ? 0x0 : static_cast<uint32_t>(ndid | (0x1 << 16));

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	ret[0] = hxcomm::vx::UTMessageToFPGA<address>(
	    address::Payload(odfi_external_access_config_reg, false));
	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(conf));
	ret[2] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[0].value(), false));
	ret[3] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(m_value.value() & 0xffff'ffff));
	ret[4] =
	    hxcomm::vx::UTMessageToFPGA<address>(address::Payload(omni_addresses[1].value(), false));
	ret[5] =
	    hxcomm::vx::UTMessageToFPGA<data>(data::Payload((m_value.value() >> 32) & 0xffff'ffff));
	ret[6] = hxcomm::vx::UTMessageToFPGA<address>(
	    address::Payload(odfi_external_access_config_reg, false));
	ret[7] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(0x0));

	return ret;
}

void ExtollOnNwNode::decode(UTMessageFromFPGARangeOmnibus const& messages)
{
	m_value = Value(
	    (static_cast<uint64_t>(messages[1].decode()) << 32) |
	    static_cast<uint64_t>(messages[0].decode()));
}

template <class Archive>
void ExtollOnNwNode::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ExtollOnNwNode)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::Extoll, 0)
CEREAL_CLASS_VERSION(fisch::vx::ExtollOnNwNode, 0)
