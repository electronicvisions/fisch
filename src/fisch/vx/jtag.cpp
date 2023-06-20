#include "fisch/vx/jtag.h"

#include "fisch/vx/detail/jtag_pll_register_coord_size.h"
#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/jtag.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hate/bitset.h"
#include "hxcomm/vx/utmessage.h"
#include <array>

namespace fisch::vx {

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

void ResetJTAGTap::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::to_fpga_jtag::Init>());
}


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

void JTAGClockScaler::encode_write(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target) const
{
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::to_fpga_jtag::Scaler>(
	    hxcomm::vx::instruction::to_fpga_jtag::Scaler::Payload(m_value.value())));
}


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

void OmnibusChipOverJTAG::encode_read(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target)
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_ADDRESS));

	hate::bitset<sizeof(Value) * CHAR_BIT + 1> addr(coord.value());
	addr.set(addr.size - 1, true);
	target(hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(addr.size), addr)));

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_REQUEST));

	target(hxcomm::vx::UTMessageToFPGA<data>(data::Payload(false, data::Payload::NumBits(3), 0)));

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_DATA));

	target(hxcomm::vx::UTMessageToFPGA<data>(data::Payload(true, data::Payload::NumBits(32), 0)));
}

void OmnibusChipOverJTAG::encode_write(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_ADDRESS));

	target(hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(33), coord.value())));

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_DATA));

	target(hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(32), m_value.value())));

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::OMNIBUS_REQUEST));

	target(hxcomm::vx::UTMessageToFPGA<data>(data::Payload(false, data::Payload::NumBits(3), 0)));
}

void OmnibusChipOverJTAG::decode(UTMessageFromFPGARangeJTAG const& messages)
{
	m_value = Value(static_cast<uint32_t>(messages[0].decode()));
}


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

void JTAGIdCode::encode_read(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target)
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::IDCODE));

	target(hxcomm::vx::UTMessageToFPGA<data>(data::Payload(true, data::Payload::NumBits(32), 0)));
}

void JTAGIdCode::decode(UTMessageFromFPGARangeJTAG const& messages)
{
	m_value = Value(static_cast<uint32_t>(messages[0].decode()));
}


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

void JTAGPLLRegister::encode_write(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::PLL_TARGET_REG));

	target(hxcomm::vx::UTMessageToFPGA<data>(data::Payload(
	    false, data::Payload::NumBits(detail::jtag_pll_register_coord_size), coord.value())));

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::SHIFT_PLL));

	target(hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(32), m_value.value())));
}


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

void JTAGPhyRegister::encode_write(
    coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
{
	using ins = hxcomm::vx::instruction::to_fpga_jtag::Ins;
	using data = hxcomm::vx::instruction::to_fpga_jtag::Data;

	target(hxcomm::vx::UTMessageToFPGA<ins>(ins::Payload((1 << 6) | (coord.toEnum() << 3) | 4)));

	target(hxcomm::vx::UTMessageToFPGA<data>(
	    data::Payload(false, data::Payload::NumBits(22), m_value.value())));
}

} // namespace fisch::vx
