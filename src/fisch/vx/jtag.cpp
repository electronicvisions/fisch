#include <array>

#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

#include "hate/bitset.h"

namespace fisch::vx {

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

} // namespace fisch::vx
