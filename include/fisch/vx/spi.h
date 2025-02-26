#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/spi.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct SPIShiftRegisterOnBoard;
struct SPIDACDataRegisterOnBoard;
struct SPIDACControlRegisterOnBoard;
} // namespace halco::hicann_dls::vx

namespace fisch {
namespace vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing the shift register over SPI.
 */
class GENPYBIND(visible) SPIShiftRegister
{
public:
	typedef halco::hicann_dls::vx::SPIShiftRegisterOnBoard coordinate_type;

	/** Shift register value. */
	typedef word_access_type::SPIShiftRegister Value GENPYBIND(visible);

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit SPIShiftRegister(Value value = Value()) : m_value(value) {}

	/**
	 * Get value.
	 * @return SPIShiftRegister value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set value.
	 * @param value SPIShiftRegister word value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SPIShiftRegister const& word) SYMBOL_VISIBLE;

	bool operator==(SPIShiftRegister const& other) const SYMBOL_VISIBLE;
	bool operator!=(SPIShiftRegister const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for reading and writing a DACData data register over SPI.
 */
class GENPYBIND(visible) SPIDACDataRegister
{
public:
	typedef halco::hicann_dls::vx::SPIDACDataRegisterOnBoard coordinate_type;

	/** DAC data value. */
	typedef word_access_type::SPIDACDataRegister Value GENPYBIND(visible);

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit SPIDACDataRegister(Value value = Value()) : m_value(value) {}

	/**
	 * Get value.
	 * @return SPIDACDataRegister value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set value.
	 * @param value SPIDACDataRegister word value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SPIDACDataRegister const& word)
	    SYMBOL_VISIBLE;

	bool operator==(SPIDACDataRegister const& other) const SYMBOL_VISIBLE;
	bool operator!=(SPIDACDataRegister const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for reading and writing a DACControl control register over SPI.
 */
class GENPYBIND(visible) SPIDACControlRegister
{
public:
	typedef halco::hicann_dls::vx::SPIDACControlRegisterOnBoard coordinate_type;

	/** DAC control value. */
	typedef word_access_type::SPIDACControlRegister Value GENPYBIND(visible);

	/**
	 * Construct an instance with a word value.
	 * @param value Value to construct instance with
	 */
	explicit SPIDACControlRegister(Value value = Value()) : m_value(value) {}

	/**
	 * Get value.
	 * @return SPIDACControlRegister value
	 */
	Value get() const SYMBOL_VISIBLE;

	/**
	 * Set value.
	 * @param value SPIDACControlRegister word value to set
	 */
	void set(Value value) SYMBOL_VISIBLE;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SPIDACControlRegister const& word)
	    SYMBOL_VISIBLE;

	bool operator==(SPIDACControlRegister const& other) const SYMBOL_VISIBLE;
	bool operator!=(SPIDACControlRegister const& other) const SYMBOL_VISIBLE;

	void encode_write(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target) const
	    GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace vx
} // namespace fisch

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::SPIShiftRegister)
FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::SPIDACDataRegister)
FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::SPIDACControlRegister)
