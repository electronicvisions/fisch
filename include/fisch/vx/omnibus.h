#pragma once

#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct OmnibusAddress;
struct PollingOmnibusBlockOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing an omnibus word.
 */
class GENPYBIND(visible) Omnibus
{
public:
	typedef halco::hicann_dls::vx::OmnibusAddress coordinate_type;

	struct Value
	{
		struct GENPYBIND(inline_base("*")) Word
		    : public halco::common::detail::BaseType<Word, uint32_t>
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
		constexpr explicit Value() : word(), byte_enables({true, true, true, true}) {}

		/**
		 * Construct an instance with a word value.
		 * @param value Word value to construct instance with
		 */
		constexpr explicit Value(Word::value_type const value) :
		    word(value), byte_enables({true, true, true, true})
		{}

		/**
		 * Construct an instance with a word value and byte enables.
		 * @param value Word value to construct instance with
		 * @param byte_enables Byte enables to construct instance with
		 */
		constexpr explicit Value(Word::value_type const value, ByteEnables const& byte_enables) :
		    word(value), byte_enables(byte_enables)
		{}

		constexpr operator Word::value_type() const
		{
			return word;
		}

		bool operator==(Value const& other) const;
		bool operator!=(Value const& other) const;

		GENPYBIND(stringstream)
		friend std::ostream& operator<<(std::ostream& os, Value const& value);

	private:
		friend class cereal::access;
		template <typename Archive>
		void serialize(Archive& ar, std::uint32_t const version);
	};

	/**
	 * Default constructor.
	 */
	explicit Omnibus();

	/**
	 * Construct an instance with a value.
	 * @param value Value to construct instance with
	 */
	explicit Omnibus(Value const& value);

	/**
	 * Get value.
	 * @return Word value
	 */
	Value const& get() const;

	/**
	 * Set value.
	 * @param value Word value to set
	 */
	void set(Value const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, Omnibus const& word);

	bool operator==(Omnibus const& other) const;
	bool operator!=(Omnibus const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 2;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeOmnibus const& messages) GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for polling block operation on a Omnibus address.
 * Compare given value from address in FPGA register file masked with expected target result.
 */
class GENPYBIND(visible) PollingOmnibusBlock
{
public:
	typedef halco::hicann_dls::vx::PollingOmnibusBlockOnFPGA coordinate_type;
	typedef bool Value;

	/**
	 * Resolve block if value is true (*address & mask == target) or if value is false (*address &
	 * mask != target).
	 */
	PollingOmnibusBlock(Value value = true);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, PollingOmnibusBlock const& config);

	bool operator==(PollingOmnibusBlock const& other) const;
	bool operator!=(PollingOmnibusBlock const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);

	Value m_value;
};

} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::Omnibus::Value::Word)
} // namespace std
