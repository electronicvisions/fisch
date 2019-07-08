#pragma once
#include "fisch/vx/decode.h"
#include "fisch/vx/genpybind.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
class access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct NullPayloadReadableOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading a payload-free tick.
 */
class GENPYBIND(visible) NullPayloadReadable
{
public:
	typedef halco::hicann_dls::vx::NullPayloadReadableOnFPGA coordinate_type;

	/** Default constructor. */
	NullPayloadReadable();

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, NullPayloadReadable const& id);

	bool operator==(NullPayloadReadable const& other) const;
	bool operator!=(NullPayloadReadable const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(UTMessageFromFPGARangeLoopback const& messages) GENPYBIND(hidden);

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace fisch::vx
