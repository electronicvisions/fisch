#pragma once
#include "fisch/vx/container.h"
#include "fisch/vx/traits.h"
#include "fisch/vx/word_access_type.h"
#include "hate/type_list.h"

namespace fisch::vx {

namespace detail {

template <typename Container, typename Value>
void maybe_set_value(Container& container, Value const& value)
{
	if constexpr (HasValue<Container>::value) {
		container.set(value);
	}
}

template <typename Container, typename Value>
void maybe_get_value(Container const& container, Value& value)
{
	if constexpr (HasValue<Container>::value) {
		value = container.get();
	}
}

} // namespace detail

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	/**                                                                                            \
	 * Cast container to its word value.                                                           \
	 */                                                                                            \
	inline word_access_type::Name container_cast(Type const& container)                            \
	{                                                                                              \
		word_access_type::Name value;                                                              \
		detail::maybe_get_value(container, value);                                                 \
		return value;                                                                              \
	}                                                                                              \
                                                                                                   \
	/**                                                                                            \
	 * Cast word value to to its container.                                                        \
	 */                                                                                            \
	inline Type container_cast(word_access_type::Name const& value)                                \
	{                                                                                              \
		Type container;                                                                            \
		detail::maybe_set_value(container, value);                                                 \
		return container;                                                                          \
	}
#include "fisch/vx/container.def"

} // namespace fisch::vx
