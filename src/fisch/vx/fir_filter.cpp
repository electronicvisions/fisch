#include "fisch/vx/fir_filter.h"

#include <array>

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/common/cerealization_geometry.h"
#include "halco/hicann-dls/vx/fir_filter.h"
#include "hate/bitset.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

FIRFilterRegister::FIRFilterRegister(Value const value) : m_data(value) {}

FIRFilterRegister::Value FIRFilterRegister::get() const
{
	return m_data;
}

void FIRFilterRegister::set(Value const value)
{
	m_data = value;
}

std::ostream& operator<<(std::ostream& os, FIRFilterRegister const& word)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << word.m_data.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << word.m_data.value();
	hate::bitset<24> bits(word.m_data.value());
	os << "FIRFilterRegister(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";
	return os;
}

bool FIRFilterRegister::operator==(FIRFilterRegister const& other) const
{
	return (m_data == other.m_data);
}

bool FIRFilterRegister::operator!=(FIRFilterRegister const& other) const
{
	return !(*this == other);
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, FIRFilterRegister::encode_write_ut_message_count>
FIRFilterRegister::encode_write(coordinate_type const& /*coord*/) const
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;

	auto addr = Omnibus::coordinate_type(fir_filter_over_omnibus_mask);
	auto addr_end = Omnibus::coordinate_type(addr + 1);

	// The FIR filter accepts data in the lowest byte of a word corresponding to a single
	// omnibus address, until the bit 8 set. Then the collected data is then loaded via a trigger
	// signal.
	for (size_t i = 0; i < 11; ++i) {
		auto encoded =
		    Omnibus(Omnibus::Value((m_data.value() >> (i * 2)) & 0x3)).encode_write(addr);
		ret[i * 2] = encoded[0];
		ret[i * 2 + 1] = encoded[1];
	}
	auto encoded =
	    Omnibus(Omnibus::Value(((m_data.value() >> 22) & 0x3) + 0x100)).encode_write(addr);
	ret[22] = encoded[0];
	ret[23] = encoded[1];
	auto encoded_end = Omnibus().encode_write(addr_end);
	ret[24] = encoded_end[0];
	ret[25] = encoded_end[1];

	return ret;
}

template <class Archive>
void FIRFilterRegister::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_data));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(FIRFilterRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::FIRFilterRegister, 0)
