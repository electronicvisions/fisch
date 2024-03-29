#include "fisch/vx/word_access/type/omnibus.h"

#ifndef __ppu__
#include "hate/bitset.h"
#include "hate/join.h"
#endif

namespace fisch::vx::word_access_type GENPYBIND_MODULE {

#ifndef __ppu__
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
#endif

bool Omnibus::operator==(Omnibus const& other) const
{
#ifndef __ppu__
	return (word == other.word) && (byte_enables == other.byte_enables);
#else
	bool ret = (word == other.word);
	for (size_t i = 0; i < byte_enables.size(); ++i) {
		ret = ret && (byte_enables[i] == other.byte_enables[i]);
	}
	return ret;
#endif
}

bool Omnibus::operator!=(Omnibus const& other) const
{
	return !(*this == other);
}

} // namespace fisch::vx::word_access_type
