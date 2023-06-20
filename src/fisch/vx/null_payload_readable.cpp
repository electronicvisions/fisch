#include "fisch/vx/null_payload_readable.h"

#include "fisch/vx/encode.h"
#include "hxcomm/vx/utmessage.h"
#include <array>

namespace fisch::vx {

std::ostream& operator<<(std::ostream& os, NullPayloadReadable const& /*tick*/)
{
	os << "NullPayloadReadable()";
	return os;
}

bool NullPayloadReadable::operator==(NullPayloadReadable const& /*other*/) const
{
	return true;
}

bool NullPayloadReadable::operator!=(NullPayloadReadable const& other) const
{
	return !(*this == other);
}

void NullPayloadReadable::encode_read(
    coordinate_type const& /* coord */, UTMessageToFPGABackEmplacer& target)
{
	target(hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Loopback>(
	    hxcomm::vx::instruction::system::Loopback::tick));
}

void NullPayloadReadable::decode(UTMessageFromFPGARangeLoopback const&) {}

} // namespace fisch::vx
