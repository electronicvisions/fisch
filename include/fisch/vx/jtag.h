#pragma once

#include "halco/common/geometry.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/utmessage.h"

#include "fisch/vx/omnibus.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx {

/**
 * Container for resetting JTAG state-machine.
 */
class ResetJTAGTap
{
public:
	typedef halco::hicann_dls::vx::JTAGOnDLS coordinate_type;

	/** Default constructor. */
	ResetJTAGTap();

	bool operator==(ResetJTAGTap const& other) const;
	bool operator!=(ResetJTAGTap const& other) const;

	constexpr static size_t encode_read_ut_message_count = 0;
	constexpr static size_t encode_write_ut_message_count = 1;
	constexpr static size_t decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

private:
	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container writing JTAG clock-scaler value.
 */
class JTAGClockScaler
{
public:
	typedef halco::hicann_dls::vx::JTAGOnDLS coordinate_type;

	/** Clock-scaler value type. The JTAG clock scales with 1 / (value + 2). */
	struct Value : public halco::common::detail::RantWrapper<Value, uint_fast16_t, 255, 0>
	{
		explicit Value(uintmax_t const value = 0) : rant_t(value) {}
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

	bool operator==(JTAGClockScaler const& other) const;
	bool operator!=(JTAGClockScaler const& other) const;

	constexpr static size_t encode_read_ut_message_count = 0;
	constexpr static size_t encode_write_ut_message_count = 1;
	constexpr static size_t decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

private:
	Value m_value;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container for reading and writing an omnibus word over JTAG.
 */
class OmnibusOnChipOverJTAG
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

	bool operator==(OmnibusOnChipOverJTAG const& other) const;
	bool operator!=(OmnibusOnChipOverJTAG const& other) const;

	constexpr static size_t encode_read_ut_message_count = 6;
	constexpr static size_t encode_write_ut_message_count = 6;
	constexpr static size_t decode_ut_message_count = 1;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

private:
	value_type m_data;

	friend class cereal::access;
	template <class Archive>
	void cerealize(Archive& ar);
};


/**
 * Container for reading the JTAG IDCODE.
 */
class JTAGIdCode
{
public:
	typedef halco::hicann_dls::vx::JTAGOnDLS coordinate_type;

	/** JTAG IDCODE value type. */
	struct Value : public halco::common::detail::BaseType<Value, uint32_t>
	{
		explicit Value(value_type const value = 0) : base_t(value) {}
	};

	/** Default constructor. */
	JTAGIdCode();

	/**
	 * Get JTAG IDCODE.
	 * @return Value
	 */
	Value get() const;

	bool operator==(JTAGIdCode const& other) const;
	bool operator!=(JTAGIdCode const& other) const;

	constexpr static size_t encode_read_ut_message_count = 2;
	constexpr static size_t encode_write_ut_message_count = 0;
	constexpr static size_t decode_ut_message_count = 1;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

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
class JTAGPLLRegister
{
public:
	typedef halco::hicann_dls::vx::PLLRegisterOnDLS coordinate_type;

	/** PLL register value type. */
	struct Value : public halco::common::detail::BaseType<Value, uint32_t>
	{
		explicit Value(value_type const value = 0) : base_t(value) {}
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

	bool operator==(JTAGPLLRegister const& other) const;
	bool operator!=(JTAGPLLRegister const& other) const;

	constexpr static size_t encode_read_ut_message_count = 0;
	constexpr static size_t encode_write_ut_message_count = 4;
	constexpr static size_t decode_ut_message_count = 0;

	static std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
	encode_read(coordinate_type const& coord);
	std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const;
	void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const&
	                messages);

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
} // namespace std
