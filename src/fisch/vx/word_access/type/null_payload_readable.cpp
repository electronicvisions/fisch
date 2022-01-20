#include "fisch/vx/word_access/type/null_payload_readable.h"

namespace fisch::vx::word_access_type {

bool NullPayloadReadable::operator==(NullPayloadReadable const& /* other */) const
{
	return true;
}

bool NullPayloadReadable::operator!=(NullPayloadReadable const& /* other */) const
{
	return false;
}

} // namespace fisch::vx::word_access_type
