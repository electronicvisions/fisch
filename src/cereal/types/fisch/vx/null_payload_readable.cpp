#include "fisch/vx/null_payload_readable.h"

#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <class Archive>
void NullPayloadReadable::serialize(Archive&, std::uint32_t const)
{}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(NullPayloadReadable)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::NullPayloadReadable, 0)
