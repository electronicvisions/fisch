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
struct OmnibusChipAddress;
struct OmnibusFPGAAddress;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading and writing an omnibus word on the Chip.
 */
class GENPYBIND(visible) OmnibusChip
{
public:
	typedef halco::hicann_dls::vx::OmnibusChipAddress coordinate_type;
	typedef OmnibusData Value;

	/**
	 * Construct an instance with a word value.
	 * @param value OmnibusChip word value to construct instance with
	 */
	OmnibusChip(Value value = Value());

	/**
	 * Get value.
	 * @return OmnibusChip word value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value OmnibusChip word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, OmnibusChip const& word);

	bool operator==(OmnibusChip const& other) const;
	bool operator!=(OmnibusChip const& other) const;

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
	void serialize(Archive& ar);
};


/**
 * Container for reading and writing an omnibus word on the FPGA.
 */
class GENPYBIND(visible) OmnibusFPGA
{
public:
	typedef halco::hicann_dls::vx::OmnibusFPGAAddress coordinate_type;
	typedef OmnibusData Value;

	/**
	 * Construct an instance with a word value.
	 * @param value OmnibusFPGA word value to construct instance with
	 */
	OmnibusFPGA(Value value = Value());

	/**
	 * Get value.
	 * @return OmnibusFPGA word value
	 */
	Value get() const;

	/**
	 * Set value.
	 * @param value OmnibusFPGA word value to set
	 */
	void set(Value value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, OmnibusFPGA const& word);

	bool operator==(OmnibusFPGA const& other) const;
	bool operator!=(OmnibusFPGA const& other) const;

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
	void serialize(Archive& ar);
};

} // namespace fisch::vx
