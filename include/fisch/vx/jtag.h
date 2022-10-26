#pragma once
#include "fisch/vx/constants.h"
#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/jtag.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct ResetJTAGTapOnDLS;
struct JTAGClockScalerOnDLS;
struct OmnibusChipOverJTAGAddress;
struct JTAGIdCodeOnDLS;
struct JTAGPLLRegisterOnDLS;
struct JTAGPhyRegisterOnDLS;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for resetting JTAG state-machine.
 */
class GENPYBIND(visible) ResetJTAGTap
{
public:
	typedef halco::hicann_dls::vx::ResetJTAGTapOnDLS coordinate_type;

	/** Default constructor. */
	ResetJTAGTap();

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, ResetJTAGTap const& reset);

	bool operator==(ResetJTAGTap const& other) const;
	bool operator!=(ResetJTAGTap const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container writing JTAG clock-scaler value.
 */
class GENPYBIND(visible) JTAGClockScaler
{
public:
	typedef halco::hicann_dls::vx::JTAGClockScalerOnDLS coordinate_type;

	/** Clock-scaler value type. The JTAG clock scales with 1 / (value + 2). */
	typedef word_access_type::JTAGClockScaler Value GENPYBIND(visible);

	/**
	 * Construct scaler with value.
	 * @param value Value to construct scaler with
	 */
	explicit JTAGClockScaler(Value value = Value());

	/**
	 * Get clock-scaler value.
	 * @return Clock-scaler value
	 */
	Value get() const;

	/**
	 * Set clock-scaler value.
	 * @param value Clock-scaler value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGClockScaler const& scaler);

	bool operator==(JTAGClockScaler const& other) const;
	bool operator!=(JTAGClockScaler const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for reading and writing an omnibus word over JTAG.
 */
class GENPYBIND(visible) OmnibusChipOverJTAG
{
public:
	typedef halco::hicann_dls::vx::OmnibusChipOverJTAGAddress coordinate_type;

	typedef word_access_type::OmnibusChipOverJTAG Value GENPYBIND(visible);

	/**
	 * Construct an instance with a word value.
	 * @param value Omnibus word value to construct instance with
	 */
	explicit OmnibusChipOverJTAG(Value value = Value());

	/**
	 * Get value.
	 * @return Omnibus word value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value Omnibus word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, OmnibusChipOverJTAG const& word);

	bool operator==(OmnibusChipOverJTAG const& other) const;
	bool operator!=(OmnibusChipOverJTAG const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 6;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 6;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeJTAG const& messages) GENPYBIND(hidden);

private:
	Value m_data;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for reading the JTAG IDCODE.
 */
class GENPYBIND(visible) JTAGIdCode
{
public:
	typedef halco::hicann_dls::vx::JTAGIdCodeOnDLS coordinate_type;

	/** JTAG IDCODE value type. */
	typedef word_access_type::JTAGIdCode Value GENPYBIND(visible);

	/**
	 * Construct IDCODE with value.
	 * @param value Value to use
	 */
	explicit JTAGIdCode(Value value = Value());

	/**
	 * Get JTAG IDCODE.
	 * @return Value
	 */
	Value get() const;

	/**
	 * Set JTAG IDCODE.
	 * @param value Value
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGIdCode const& id);

	bool operator==(JTAGIdCode const& other) const;
	bool operator!=(JTAGIdCode const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 2;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeJTAG const& messages) GENPYBIND(hidden);

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * Container for writing a PLL register.
 * Writing this container does not lead to an update in the registerfile of omnibus access to the
 * same PLL configuration. Data written with this container can therefore never be read out again.
 */
class GENPYBIND(visible) JTAGPLLRegister
{
public:
	typedef halco::hicann_dls::vx::JTAGPLLRegisterOnDLS coordinate_type;

	/** PLL register value type. */
	typedef word_access_type::JTAGPLLRegister Value GENPYBIND(visible);

	/**
	 * Construct register by its value.
	 * @param value Value to set on cosntruction
	 */
	explicit JTAGPLLRegister(Value value = Value());

	/**
	 * Get register value.
	 * @return Value to get
	 */
	Value get() const;

	/**
	 * Set register value.
	 * @param value Value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGPLLRegister const& reg);

	bool operator==(JTAGPLLRegister const& other) const;
	bool operator!=(JTAGPLLRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 4;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

/**
 * Container writing Phy configuration on the Chip.
 */
class GENPYBIND(visible) JTAGPhyRegister
{
public:
	typedef halco::hicann_dls::vx::JTAGPhyRegisterOnDLS coordinate_type;

	/** Register value type. */
	typedef word_access_type::JTAGPhyRegister Value GENPYBIND(visible);

	/**
	 * Construct register with value.
	 * @param value Value to construct register with
	 */
	explicit JTAGPhyRegister(Value value = Value());

	/**
	 * Get register value.
	 * @return Register value
	 */
	Value get() const;

	/**
	 * Set register value.
	 * @param value Register value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGPhyRegister const& reg);

	bool operator==(JTAGPhyRegister const& other) const;
	bool operator!=(JTAGPhyRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 2;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

private:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx
