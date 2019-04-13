#include <array>

#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

#include "hate/bitset.h"

namespace fisch::vx {

ResetJTAGTap::ResetJTAGTap() {}

std::ostream& operator<<(std::ostream& os, ResetJTAGTap const& /*reset*/)
{
	os << "ResetJTAGTap()";
	return os;
}

bool ResetJTAGTap::operator==(ResetJTAGTap const& /*other*/) const
{
	return true;
}

bool ResetJTAGTap::operator!=(ResetJTAGTap const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, ResetJTAGTap::encode_read_ut_message_count>
ResetJTAGTap::encode_read(coordinate_type const& /* coord */)
{
	return {};
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, ResetJTAGTap::encode_write_ut_message_count>
ResetJTAGTap::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::ut_message_to_fpga<hxcomm::vx::instruction::to_fpga_jtag::init>()};
}

void ResetJTAGTap::decode(std::array<
                          hxcomm::vx::ut_message_from_fpga_variant,
                          decode_ut_message_count> const& /* messages */)
{}

template <class Archive>
void ResetJTAGTap::cerealize(Archive& /*ar*/)
{}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ResetJTAGTap)

JTAGClockScaler::JTAGClockScaler() : m_value() {}

JTAGClockScaler::JTAGClockScaler(Value const value) : m_value(value) {}

JTAGClockScaler::Value JTAGClockScaler::get() const
{
	return m_value;
}

void JTAGClockScaler::set(Value const& value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, JTAGClockScaler const& scaler)
{
	std::stringstream ss;
	ss << std::dec << scaler.m_value.value();
	os << "JTAGClockScaler(" << ss.str() << ")";
	return os;
}

bool JTAGClockScaler::operator==(JTAGClockScaler const& other) const
{
	return (m_value == other.m_value);
}

bool JTAGClockScaler::operator!=(JTAGClockScaler const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGClockScaler::encode_read_ut_message_count>
JTAGClockScaler::encode_read(coordinate_type const& /* coord */)
{
	return {};
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGClockScaler::encode_write_ut_message_count>
JTAGClockScaler::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::ut_message_to_fpga<hxcomm::vx::instruction::to_fpga_jtag::scaler>(
	    hxcomm::vx::instruction::to_fpga_jtag::scaler::payload_type(m_value.value()))};
}

void JTAGClockScaler::decode(std::array<
                             hxcomm::vx::ut_message_from_fpga_variant,
                             decode_ut_message_count> const& /* messages */)
{}

template <class Archive>
void JTAGClockScaler::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGClockScaler)

OmnibusOnChipOverJTAG::OmnibusOnChipOverJTAG() : m_data() {}

OmnibusOnChipOverJTAG::OmnibusOnChipOverJTAG(value_type const& value) : m_data(value) {}

OmnibusOnChipOverJTAG::value_type OmnibusOnChipOverJTAG::get() const
{
	return m_data;
}

void OmnibusOnChipOverJTAG::set(value_type const& value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, OmnibusOnChipOverJTAG const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<sizeof(typename OmnibusOnChipOverJTAG::value_type::value_type) * CHAR_BIT> bits(
	    word.m_data.value());
	os << "OmnibusOnChipOverJTAG(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool OmnibusOnChipOverJTAG::operator==(OmnibusOnChipOverJTAG const& other) const
{
	return (m_data == other.m_data);
}

bool OmnibusOnChipOverJTAG::operator!=(OmnibusOnChipOverJTAG const& other) const
{
	return !(*this == other);
}

std::array<
    hxcomm::vx::ut_message_to_fpga_variant,
    OmnibusOnChipOverJTAG::encode_read_ut_message_count>
OmnibusOnChipOverJTAG::encode_read(coordinate_type const& coord)
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count> ret;

	ret[0] = hxcomm::vx::ut_message_to_fpga<ins>(ins::OMNIBUS_ADDRESS);

	hate::bitset<sizeof(value_type) * CHAR_BIT + 1> addr(coord.value());
	addr.set(addr.size - 1, true);
	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(addr.size), addr));

	ret[2] = hxcomm::vx::ut_message_to_fpga<ins>(ins::OMNIBUS_REQUEST);

	ret[3] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(3), 0));

	ret[4] = hxcomm::vx::ut_message_to_fpga<ins>(ins::OMNIBUS_DATA);

	ret[5] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(true, data::payload_type::NumBits(32), 0));
	return ret;
}

std::array<
    hxcomm::vx::ut_message_to_fpga_variant,
    OmnibusOnChipOverJTAG::encode_write_ut_message_count>
OmnibusOnChipOverJTAG::encode_write(coordinate_type const& coord) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;

	ret[0] = hxcomm::vx::ut_message_to_fpga<ins>(ins::OMNIBUS_ADDRESS);

	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(33), coord.value()));

	ret[2] = hxcomm::vx::ut_message_to_fpga<ins>(ins::OMNIBUS_DATA);

	ret[3] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(32), m_data.value()));

	ret[4] = hxcomm::vx::ut_message_to_fpga<ins>(ins::OMNIBUS_REQUEST);

	ret[5] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(3), 0));
	return ret;
}

void OmnibusOnChipOverJTAG::decode(
    std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const& messages)
{
	using data = hxcomm::vx::instruction::jtag_from_hicann::data;

	auto data_message = boost::get<hxcomm::vx::ut_message_from_fpga<data> >(messages[0]);
	m_data = value_type(static_cast<uint32_t>(data_message.get_payload()));
}

template <class Archive>
void OmnibusOnChipOverJTAG::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(OmnibusOnChipOverJTAG)

JTAGIdCode::JTAGIdCode() : m_value() {}

JTAGIdCode::Value JTAGIdCode::get() const
{
	return m_value;
}

std::ostream& operator<<(std::ostream& os, JTAGIdCode const& id)
{
	std::stringstream ss;
	ss << "0x" << std::hex << id.m_value.value();
	os << "JTAGIdCode(" << ss.str() << ")";
	return os;
}

bool JTAGIdCode::operator==(JTAGIdCode const& other) const
{
	return (m_value == other.m_value);
}

bool JTAGIdCode::operator!=(JTAGIdCode const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGIdCode::encode_read_ut_message_count>
JTAGIdCode::encode_read(coordinate_type const& /* coord */)
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count> ret;
	ret[0] = hxcomm::vx::ut_message_to_fpga<ins>(ins::IDCODE);

	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(true, data::payload_type::NumBits(32), 0));
	return ret;
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGIdCode::encode_write_ut_message_count>
JTAGIdCode::encode_write(coordinate_type const& /* coord */) const
{
	return {};
}

void JTAGIdCode::decode(
    std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const& messages)
{
	using data = hxcomm::vx::instruction::jtag_from_hicann::data;

	m_value = Value(static_cast<uint32_t>(
	    boost::get<hxcomm::vx::ut_message_from_fpga<data> >(messages[0]).decode()));
}

template <class Archive>
void JTAGIdCode::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGIdCode)

JTAGPLLRegister::JTAGPLLRegister() : m_value() {}

JTAGPLLRegister::JTAGPLLRegister(Value const value) : m_value(value) {}

void JTAGPLLRegister::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, JTAGPLLRegister const& reg)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << reg.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << reg.m_value.value();
	hate::bitset<sizeof(typename JTAGPLLRegister::Value::value_type) * CHAR_BIT> bits(
	    reg.m_value.value());
	os << "JTAGPLLRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool JTAGPLLRegister::operator==(JTAGPLLRegister const& other) const
{
	return (m_value == other.m_value);
}

bool JTAGPLLRegister::operator!=(JTAGPLLRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGPLLRegister::encode_read_ut_message_count>
JTAGPLLRegister::encode_read(coordinate_type const& /* coord */)
{
	return {};
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGPLLRegister::encode_write_ut_message_count>
JTAGPLLRegister::encode_write(coordinate_type const& coord) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;
	ret[0] = hxcomm::vx::ut_message_to_fpga<ins>(ins::PLL_TARGET_REG);

	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(3), coord.value()));

	ret[2] = hxcomm::vx::ut_message_to_fpga<ins>(ins::SHIFT_PLL);

	ret[3] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(32), m_value.value()));
	return ret;
}

void JTAGPLLRegister::decode(
    std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
    /*messages*/)
{}

template <class Archive>
void JTAGPLLRegister::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGPLLRegister)

JTAGPhyRegister::JTAGPhyRegister() : m_value() {}

JTAGPhyRegister::JTAGPhyRegister(Value const value) : m_value(value) {}

JTAGPhyRegister::Value JTAGPhyRegister::get() const
{
	return m_value;
}

void JTAGPhyRegister::set(Value const& value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, JTAGPhyRegister const& reg)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << reg.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << reg.m_value.value();
	hate::bitset<22> bits(reg.m_value.value());
	os << "JTAGPhyRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool JTAGPhyRegister::operator==(JTAGPhyRegister const& other) const
{
	return (m_value == other.m_value);
}

bool JTAGPhyRegister::operator!=(JTAGPhyRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGPhyRegister::encode_read_ut_message_count>
JTAGPhyRegister::encode_read(coordinate_type const& /* coord */)
{
	return {};
}

std::array<hxcomm::vx::ut_message_to_fpga_variant, JTAGPhyRegister::encode_write_ut_message_count>
JTAGPhyRegister::encode_write(coordinate_type const& coord) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::data;

	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> ret;
	ret[0] = hxcomm::vx::ut_message_to_fpga<ins>(
	    ins::payload_type((1 << 6) | (coord.toEnum() << 3) | 4));

	ret[1] = hxcomm::vx::ut_message_to_fpga<data>(
	    data::payload_type(false, data::payload_type::NumBits(22), m_value.value()));

	return ret;
}

void JTAGPhyRegister::decode(std::array<
                             hxcomm::vx::ut_message_from_fpga_variant,
                             decode_ut_message_count> const& /* messages */)
{}

template <class Archive>
void JTAGPhyRegister::cerealize(Archive& ar)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGPhyRegister)

} // namespace fisch::vx
