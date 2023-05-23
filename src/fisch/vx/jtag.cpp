#include "fisch/vx/jtag.h"

#include "fisch/cerealization.h"
#include "fisch/vx/detail/jtag_pll_register_coord_size.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/jtag.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hate/bitset.h"
#include "hxcomm/vx/utmessage.h"
#include <array>

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

std::array<hxcomm::vx::UTMessageToFPGAVariant, ResetJTAGTap::encode_write_ut_message_count>
ResetJTAGTap::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::to_fpga_jtag::Init>()};
}

template <class Archive>
void ResetJTAGTap::serialize(Archive& /*ar*/, std::uint32_t const)
{}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(ResetJTAGTap)


JTAGClockScaler::JTAGClockScaler(Value const value) : m_value(value) {}

JTAGClockScaler::Value JTAGClockScaler::get() const
{
	return m_value;
}

void JTAGClockScaler::set(Value const value)
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, JTAGClockScaler::encode_write_ut_message_count>
JTAGClockScaler::encode_write(coordinate_type const& /* coord */) const
{
	return {hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::to_fpga_jtag::Scaler>(
	    hxcomm::vx::instruction::to_fpga_jtag::Scaler::Payload(m_value.value()))};
}

template <class Archive>
void JTAGClockScaler::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGClockScaler)


OmnibusChipOverJTAG::OmnibusChipOverJTAG(Value const value) : m_value(value) {}

OmnibusChipOverJTAG::Value OmnibusChipOverJTAG::get() const
{
	return m_value;
}

void OmnibusChipOverJTAG::set(Value const value)
{
	m_value = value;
}

std::ostream& operator<<(std::ostream& os, OmnibusChipOverJTAG const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_value.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_value.value();
	hate::bitset<sizeof(typename OmnibusChipOverJTAG::Value::value_type) * CHAR_BIT> bits(
	    word.m_value.value());
	os << "OmnibusChipOverJTAG(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool OmnibusChipOverJTAG::operator==(OmnibusChipOverJTAG const& other) const
{
	return (m_value == other.m_value);
}

bool OmnibusChipOverJTAG::operator!=(OmnibusChipOverJTAG const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, OmnibusChipOverJTAG::encode_read_ut_message_count>
OmnibusChipOverJTAG::encode_read(coordinate_type const& coord)
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;

	ret[0] = hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_ADDRESS);

	hate::bitset<sizeof(Value) * CHAR_BIT + 1> addr(coord.value());
	addr.set(addr.size - 1, true);
	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(addr.size), addr));

	ret[2] = hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_REQUEST);

	ret[3] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(false, data::Payload::NumBits(3), 0));

	ret[4] = hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_DATA);

	ret[5] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(true, data::Payload::NumBits(32), 0));
	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, OmnibusChipOverJTAG::encode_write_ut_message_count>
OmnibusChipOverJTAG::encode_write(coordinate_type const& coord) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	ret[0] = hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_ADDRESS);

	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(33), coord.value()));

	ret[2] = hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_DATA);

	ret[3] = hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(32), m_value.value()));

	ret[4] = hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_REQUEST);

	ret[5] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(false, data::Payload::NumBits(3), 0));
	return ret;
}

void OmnibusChipOverJTAG::decode(UTMessageFromFPGARangeJTAG const& messages)
{
	m_value = Value(static_cast<uint32_t>(messages[0].decode()));
}

template <class Archive>
void OmnibusChipOverJTAG::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(OmnibusChipOverJTAG)


JTAGIdCode::JTAGIdCode(Value value) : m_value(value) {}

JTAGIdCode::Value JTAGIdCode::get() const
{
	return m_value;
}

void JTAGIdCode::set(Value const value)
{
	m_value = value;
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, JTAGIdCode::encode_read_ut_message_count>
JTAGIdCode::encode_read(coordinate_type const& /* coord */)
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;
	ret[0] = hxcomm::vx::UTMessageToFPGA<ins>(ins::IDCODE);

	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(true, data::Payload::NumBits(32), 0));
	return ret;
}

void JTAGIdCode::decode(UTMessageFromFPGARangeJTAG const& messages)
{
	m_value = Value(static_cast<uint32_t>(messages[0].decode()));
}

template <class Archive>
void JTAGIdCode::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGIdCode)


JTAGPLLRegister::JTAGPLLRegister(Value const value) : m_value(value) {}

JTAGPLLRegister::Value JTAGPLLRegister::get() const
{
	return m_value;
}

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

std::array<hxcomm::vx::UTMessageToFPGAVariant, JTAGPLLRegister::encode_write_ut_message_count>
JTAGPLLRegister::encode_write(coordinate_type const& coord) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;
	ret[0] = hxcomm::vx::UTMessageToFPGA<ins>(ins::PLL_TARGET_REG);

	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(
	    false, data::Payload::NumBits(detail::jtag_pll_register_coord_size), coord.value()));

	ret[2] = hxcomm::vx::UTMessageToFPGA<ins>(ins::SHIFT_PLL);

	ret[3] = hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(32), m_value.value()));
	return ret;
}

template <class Archive>
void JTAGPLLRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGPLLRegister)


JTAGPhyRegister::JTAGPhyRegister(Value const value) : m_value(value) {}

JTAGPhyRegister::Value JTAGPhyRegister::get() const
{
	return m_value;
}

void JTAGPhyRegister::set(Value const value)
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

std::array<hxcomm::vx::UTMessageToFPGAVariant, JTAGPhyRegister::encode_write_ut_message_count>
JTAGPhyRegister::encode_write(coordinate_type const& coord) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;
	ret[0] = hxcomm::vx::UTMessageToFPGA<ins>(ins::Payload((1 << 6) | (coord.toEnum() << 3) | 4));

	ret[1] = hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(22), m_value.value()));

	return ret;
}

template <class Archive>
void JTAGPhyRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(JTAGPhyRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::ResetJTAGTap, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGClockScaler, 0)
CEREAL_CLASS_VERSION(fisch::vx::OmnibusChipOverJTAG, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGIdCode, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGPLLRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::JTAGPhyRegister, 0)
