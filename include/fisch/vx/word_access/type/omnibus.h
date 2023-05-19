#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"
#include "hate/visibility.h"

namespace cereal {
struct access;
} // namespace cereal

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(visible) Omnibus
{
	struct GENPYBIND(inline_base("*")) Word : public halco::common::detail::BaseType<Word, uint32_t>
	{
		constexpr explicit Word(value_type const value = 0) GENPYBIND(implicit_conversion) :
		    base_t(value)
		{}
	};

	typedef Word::value_type value_type;

	typedef std::array<bool, sizeof(Word::value_type)> ByteEnables;

	Word word;
	ByteEnables byte_enables;

	/**
	 * Default constructor.
	 */
	constexpr explicit Omnibus() : word(), byte_enables({true, true, true, true}) {}

	/**
	 * Construct an instance with a word value.
	 * @param value Word value to construct instance with
	 */
	constexpr explicit Omnibus(Word::value_type const value) :
	    word(value), byte_enables({true, true, true, true})
	{}

	/**
	 * Construct an instance with a word value and byte enables.
	 * @param value Word value to construct instance with
	 * @param byte_enables Byte enables to construct instance with
	 */
	constexpr explicit Omnibus(Word::value_type const value, ByteEnables const& byte_enables) :
	    word(value), byte_enables(byte_enables)
	{}

	constexpr operator Word::value_type() const
	{
		return word;
	}

	bool operator==(Omnibus const& other) const SYMBOL_VISIBLE;
	bool operator!=(Omnibus const& other) const SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Omnibus const& value) SYMBOL_VISIBLE;

private:
	friend struct cereal::access;
	template <typename Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


struct GENPYBIND(inline_base("*")) PollingOmnibusBlock
    : public halco::common::detail::BaseType<PollingOmnibusBlock, bool>
{
	constexpr explicit PollingOmnibusBlock(bool value = false) GENPYBIND(implicit_conversion) :
	    base_t(value)
	{}
};

} // namespace word_access_type
} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::Omnibus::Word)
} // namespace std

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::word_access_type::Omnibus)
