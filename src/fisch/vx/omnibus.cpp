#include <array>

#include "hxcomm/vx/utmessage.h"

#include "fisch/cerealization.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"

#include "hate/bitset.h"

namespace fisch::vx {

#define OMNIBUS_X_IMPL(Name, AddressMask)                                                          \
                                                                                                   \
	Name::Name() : m_data() {}                                                                     \
                                                                                                   \
	Name::Name(value_type const value) : m_data(value) {}                                          \
                                                                                                   \
	Name::value_type Name::get() const { return m_data; }                                          \
                                                                                                   \
	void Name::set(value_type const value) { m_data = value; }                                     \
                                                                                                   \
	std::ostream& operator<<(std::ostream& os, Name const& word)                                   \
	{                                                                                              \
		std::stringstream ss_d;                                                                    \
		ss_d << "0d" << std::dec << word.m_data.value();                                           \
		std::stringstream ss_x;                                                                    \
		ss_x << "0x" << std::hex << word.m_data.value();                                           \
		hate::bitset<sizeof(typename Name::value_type::value_type) * CHAR_BIT> bits(               \
		    word.m_data.value());                                                                  \
		os << #Name << "(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits << ")";             \
		return os;                                                                                 \
	}                                                                                              \
                                                                                                   \
	bool Name::operator==(Name const& other) const { return (m_data == other.m_data); }            \
                                                                                                   \
	bool Name::operator!=(Name const& other) const { return !(*this == other); }                   \
                                                                                                   \
	std::array<hxcomm::vx::UTMessageToFPGAVariant, Name::encode_read_ut_message_count>             \
	Name::encode_read(coordinate_type const& coord)                                                \
	{                                                                                              \
		using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;                         \
                                                                                                   \
		std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;          \
                                                                                                   \
		ret[0] = hxcomm::vx::UTMessageToFPGA<address>(                                             \
		    address::Payload(coord.value() | AddressMask, true));                                  \
		return ret;                                                                                \
	}                                                                                              \
                                                                                                   \
	std::array<hxcomm::vx::UTMessageToFPGAVariant, Name::encode_write_ut_message_count>            \
	Name::encode_write(coordinate_type const& coord) const                                         \
	{                                                                                              \
		using address = hxcomm::vx::instruction::omnibus_to_fpga::Address;                         \
		using data = hxcomm::vx::instruction::omnibus_to_fpga::Data;                               \
                                                                                                   \
		std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> ret;         \
                                                                                                   \
		ret[0] = hxcomm::vx::UTMessageToFPGA<address>(                                             \
		    address::Payload(coord.value() | AddressMask, false));                                 \
                                                                                                   \
		ret[1] = hxcomm::vx::UTMessageToFPGA<data>(data::Payload(m_data.value()));                 \
                                                                                                   \
		return ret;                                                                                \
	}                                                                                              \
                                                                                                   \
	void Name::decode(                                                                             \
	    std::array<hxcomm::vx::UTMessageFromFPGAVariant, decode_ut_message_count> const& messages) \
	{                                                                                              \
		using data = hxcomm::vx::instruction::omnibus_from_fpga::Data;                             \
                                                                                                   \
		auto data_message = boost::get<hxcomm::vx::UTMessageFromFPGA<data> >(messages[0]);         \
		m_data = value_type(static_cast<uint32_t>(data_message.get_payload()));                    \
	}                                                                                              \
                                                                                                   \
	template <class Archive>                                                                       \
	void Name::cerealize(Archive& ar)                                                              \
	{                                                                                              \
		ar(CEREAL_NVP(m_data));                                                                    \
	}                                                                                              \
                                                                                                   \
	EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Name)

OMNIBUS_X_IMPL(OmnibusChip, 0x0ul)
OMNIBUS_X_IMPL(OmnibusFPGA, executor_omnibus_mask)

#undef OMNIBUS_X_IMPL

} // namespace fisch::vx
