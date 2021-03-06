#include "fisch/vx/null_payload_readable.h"

#include <array>

#include "fisch/cerealization.h"
#include "halco/hicann-dls/vx/fpga.h"
#include "hxcomm/vx/utmessage.h"

namespace fisch::vx {

NullPayloadReadable::NullPayloadReadable() {}

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

std::array<hxcomm::vx::UTMessageToFPGAVariant, NullPayloadReadable::encode_read_ut_message_count>
NullPayloadReadable::encode_read(coordinate_type const& /* coord */)
{
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> ret;
	ret[0] = hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::system::Loopback>(
	    hxcomm::vx::instruction::system::Loopback::tick);
	return ret;
}

std::array<hxcomm::vx::UTMessageToFPGAVariant, NullPayloadReadable::encode_write_ut_message_count>
NullPayloadReadable::encode_write(coordinate_type const& /* coord */) const
{
	return {};
}

void NullPayloadReadable::decode(UTMessageFromFPGARangeLoopback const&) {}

template <class Archive>
void NullPayloadReadable::serialize(Archive&, std::uint32_t const)
{}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(NullPayloadReadable)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::NullPayloadReadable, 0)
