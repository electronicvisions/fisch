#pragma once
#include "hate/visibility.h"
#include <cstdint>

namespace cereal {

class access;
class PortableBinaryInputArchive;
class PortableBinaryOutputArchive;
class JSONInputArchive;
class JSONOutputArchive;

} // namespace cereal


#define FISCH_EXTERN_INSTANTIATE_CEREAL_SERIALIZE(CLASS_NAME)                                      \
	extern template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                    \
	    cereal::PortableBinaryOutputArchive&, std::uint32_t const);                                \
	extern template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                    \
	    cereal::PortableBinaryInputArchive&, std::uint32_t const);                                 \
	extern template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                    \
	    cereal::JSONOutputArchive&, std::uint32_t const);                                          \
	extern template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                    \
	    cereal::JSONInputArchive&, std::uint32_t const);
