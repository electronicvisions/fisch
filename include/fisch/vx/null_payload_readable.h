#pragma once
#include "fisch/cerealization.h"
#include "fisch/vx/decode.h"
#include "fisch/vx/encode_fwd.h"
#include "fisch/vx/genpybind.h"
#include "hate/visibility.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct NullPayloadReadableOnFPGA;
} // namespace halco::hicann_dls::vx

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {

/**
 * Container for reading a payload-free tick.
 */
class GENPYBIND(visible) NullPayloadReadable
{
public:
	typedef halco::hicann_dls::vx::NullPayloadReadableOnFPGA coordinate_type;

	/** Default constructor. */
	NullPayloadReadable() {}

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, NullPayloadReadable const& id) SYMBOL_VISIBLE;

	bool operator==(NullPayloadReadable const& other) const SYMBOL_VISIBLE;
	bool operator!=(NullPayloadReadable const& other) const SYMBOL_VISIBLE;

	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 1;

	static void encode_read(coordinate_type const& coord, UTMessageToFPGABackEmplacer& target)
	    GENPYBIND(hidden) SYMBOL_VISIBLE;
	void decode(UTMessageFromFPGARangeLoopback const& messages) GENPYBIND(hidden) SYMBOL_VISIBLE;

private:
	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t);
};

} // namespace vx
} // namespace fisch

FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(fisch::vx::NullPayloadReadable)
