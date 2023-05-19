#pragma once
#include <cereal/cereal.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "hate/visibility.h"

#define EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(CLASS_NAME)                                          \
	template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                           \
	    cereal::PortableBinaryOutputArchive&, std::uint32_t const);                                \
	template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                           \
	    cereal::PortableBinaryInputArchive&, std::uint32_t const);                                 \
	template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                           \
	    cereal::JSONOutputArchive&, std::uint32_t const);                                          \
	template SYMBOL_VISIBLE void CLASS_NAME ::serialize(                                           \
	    cereal::JSONInputArchive&, std::uint32_t const);
