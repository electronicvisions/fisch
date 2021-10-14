#include "fisch/vx/word_access/type/omnibus.h"

#include "fisch/cerealization.h"
#include "halco/common/cerealization_geometry.h"
#include "hate/bitset.h"
#include "hate/join.h"
#include <cereal/types/array.hpp>

namespace fisch::vx::word_access_type GENPYBIND_MODULE {

std::ostream& operator<<(std::ostream& os, Omnibus const& value)
{
	std::stringstream ss_d;
	ss_d << "0d" << std::dec << value.word.value();
	std::stringstream ss_x;
	ss_x << "0x" << std::hex << value.word.value();
	hate::bitset<sizeof(typename Omnibus::value_type) * CHAR_BIT> bits(value.word.value());
	os << "Value(" << ss_d.str() << " " << ss_x.str() << " 0b" << bits
	   << ", byte_enables: " << hate::join_string(value.byte_enables, "") << ")";
	return os;
}

bool Omnibus::operator==(Omnibus const& other) const
{
	return (word == other.word) && (byte_enables == other.byte_enables);
}

bool Omnibus::operator!=(Omnibus const& other) const
{
	return !(*this == other);
}

template <class Archive>
void Omnibus::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(word));
	ar(CEREAL_NVP(byte_enables));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(Omnibus)

} // namespace fisch::vx::word_access_type

CEREAL_CLASS_VERSION(fisch::vx::word_access_type::Omnibus, 0)
