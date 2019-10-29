#pragma once

#include "halco/common/geometry.h"
#include "hxcomm/vx/utmessage_fwd.h"

#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/omnibus_data.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct SPIShiftRegisterOnBoard;
struct SPIDACDataRegisterOnBoard;
struct SPIDACControlRegisterOnBoard;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing the shift register over SPI.
 */
class GENPYBIND(visible) SPIShiftRegister
{
public:
	typedef halco::hicann_dls::vx::SPIShiftRegisterOnBoard coordinate_type;

	/** Shift register value. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint_fast32_t, 0xffffff, 0>
	{
		constexpr explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
		    rant_t(value)
		{}
	};

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	SPIShiftRegister(Value value = Value());

	/**
	 * Get value.
	 * @return SPIShiftRegister value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value SPIShiftRegister word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SPIShiftRegister const& word);

	bool operator==(SPIShiftRegister const& other) const;
	bool operator!=(SPIShiftRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 6;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};


/**
 * Container for reading and writing a DACData data register over SPI.
 */
class GENPYBIND(visible) SPIDACDataRegister
{
public:
	typedef halco::hicann_dls::vx::SPIDACDataRegisterOnBoard coordinate_type;

	/** DAC data value. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint_fast32_t, 0xfff, 0>
	{
		constexpr explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
		    rant_t(value)
		{}
	};

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	SPIDACDataRegister(Value value = Value());

	/**
	 * Get value.
	 * @return SPIDACDataRegister value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value SPIDACDataRegister word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SPIDACDataRegister const& word);

	bool operator==(SPIDACDataRegister const& other) const;
	bool operator!=(SPIDACDataRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 8;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};


/**
 * Container for reading and writing a DACControl control register over SPI.
 */
class GENPYBIND(visible) SPIDACControlRegister
{
public:
	typedef halco::hicann_dls::vx::SPIDACControlRegisterOnBoard coordinate_type;

	/** DAC control value. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint_fast32_t, 0x1fff, 0>
	{
		constexpr explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) :
		    rant_t(value)
		{}
	};

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	SPIDACControlRegister(Value value = Value());

	/**
	 * Get value.
	 * @return SPIDACControlRegister value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value SPIDACControlRegister word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SPIDACControlRegister const& word);

	bool operator==(SPIDACControlRegister const& other) const;
	bool operator!=(SPIDACControlRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 4;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_data;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};

} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::SPIShiftRegister::Value)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::SPIDACControlRegister::Value)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::SPIDACDataRegister::Value)

} // namespace std
