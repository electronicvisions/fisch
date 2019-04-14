#pragma once

#include "halco/common/geometry.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/genpybind.h"
#include "fisch/vx/omnibus_data.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for resetting JTAG state-machine.
 */
class GENPYBIND(visible) ResetJTAGTap
{
public:
	typedef halco::hicann_dls::vx::JTAGOnDLS coordinate_type;

	/** Default constructor. */
	ResetJTAGTap();

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, ResetJTAGTap const& reset);

	bool operator==(ResetJTAGTap const& other) const;
	bool operator!=(ResetJTAGTap const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container writing JTAG clock-scaler value.
 */
class GENPYBIND(visible) JTAGClockScaler
{
public:
	typedef halco::hicann_dls::vx::JTAGOnDLS coordinate_type;

	/** Clock-scaler value type. The JTAG clock scales with 1 / (value + 2). */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint_fast16_t, 255, 0>
	{
		explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) : rant_t(value) {}
	};

	/** Default constructor. */
	JTAGClockScaler();

	/**
	 * Construct scaler with value.
	 * @param value Value to construct scaler with
	 */
	JTAGClockScaler(Value value);

	/**
	 * Get clock-scaler value.
	 * @return Clock-scaler value
	 */
	Value get() const;

	/**
	 * Set clock-scaler value.
	 * @param Clock-scaler value to set
	 */
	void set(Value const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGClockScaler const& scaler);

	bool operator==(JTAGClockScaler const& other) const;
	bool operator!=(JTAGClockScaler const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container for reading and writing an omnibus word over JTAG.
 */
class GENPYBIND(visible) OmnibusOnChipOverJTAG
{
public:
	typedef halco::hicann_dls::vx::OmnibusAddress coordinate_type;
	typedef OmnibusData value_type;

	/** Default constructor. */
	OmnibusOnChipOverJTAG();

	/**
	 * Construct an instance with a word value.
	 * @param value Omnibus word value to construct instance with
	 */
	OmnibusOnChipOverJTAG(value_type const& value);

	/**
	 * Get value.
	 * @return Omnibus word value
	 */
	value_type get() const;

	/**
	 * Set value.
	 * @param value Omnibus word value to set
	 */
	void set(value_type const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, OmnibusOnChipOverJTAG const& word);

	bool operator==(OmnibusOnChipOverJTAG const& other) const;
	bool operator!=(OmnibusOnChipOverJTAG const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 6;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 6;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	value_type m_data;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container for reading the JTAG IDCODE.
 */
class GENPYBIND(visible) JTAGIdCode
{
public:
	typedef halco::hicann_dls::vx::JTAGOnDLS coordinate_type;

	/** JTAG IDCODE value type. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::BaseType<Value, uint32_t>
	{
		explicit Value(value_type const value = 0) GENPYBIND(implicit_conversion) : base_t(value) {}
	};

	/** Default constructor. */
	JTAGIdCode();

	/**
	 * Get JTAG IDCODE.
	 * @return Value
	 */
	Value get() const;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGIdCode const& id);

	bool operator==(JTAGIdCode const& other) const;
	bool operator!=(JTAGIdCode const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 2;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container for writing a PLL register.
 * Writing this container does not lead to an update in the registerfile of omnibus access to the
 * same PLL configuration. Data written with this container can therefore never be read out again.
 */
class GENPYBIND(visible) JTAGPLLRegister
{
public:
	typedef halco::hicann_dls::vx::PLLRegisterOnDLS coordinate_type;

	/** PLL register value type. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::BaseType<Value, uint32_t>
	{
		explicit Value(value_type const value = 0) GENPYBIND(implicit_conversion) : base_t(value) {}
	};

	/** Default constructor. */
	JTAGPLLRegister();

	/**
	 * Construct register by its value.
	 * @param value Value to set on cosntruction
	 */
	JTAGPLLRegister(Value value);

	/**
	 * Set register value.
	 * @param value Value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGPLLRegister const& reg);

	bool operator==(JTAGPLLRegister const& other) const;
	bool operator!=(JTAGPLLRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 4;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};

/**
 * Container writing Phy configuration on the Chip.
 */
class GENPYBIND(visible) JTAGPhyRegister
{
public:
	typedef halco::hicann_dls::vx::PhyOnDLS coordinate_type;

	/** Register value type. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint_fast16_t, 4194303 /* 2^22-1 */, 0>
	{
		explicit Value(uintmax_t const value = 0) GENPYBIND(implicit_conversion) : rant_t(value) {}
	};

	/** Default constructor. */
	JTAGPhyRegister();

	/**
	 * Construct register with value.
	 * @param value Value to construct register with
	 */
	JTAGPhyRegister(Value value);

	/**
	 * Get register value.
	 * @return Register value
	 */
	Value get() const;

	/**
	 * Set register value.
	 * @param Register value to set
	 */
	void set(Value const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, JTAGPhyRegister const& reg);

	bool operator==(JTAGPhyRegister const& other) const;
	bool operator!=(JTAGPhyRegister const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 2;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages) GENPYBIND(hidden);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};

} // namespace fisch::vx

namespace std {
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::JTAGIdCode::Value)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::JTAGPLLRegister::Value)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::JTAGClockScaler::Value)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::JTAGPhyRegister::Value)
} // namespace std
