#pragma once
#include "fisch/vx/container.h"
#include "fisch/vx/decode.h"
#include "hate/type_list.h"

namespace fisch::vx {

namespace detail {

/**
 * @note Collection of message types omitted in documentation.
 */
typedef hate::type_list_unique_t<hate::multi_concat_t<
/// @cond DISABLED
#define LAST_PLAYBACK_CONTAINER(Name, Type) typename DecodeMessageTypeList<Type>::type
#define PLAYBACK_CONTAINER(Name, Type) typename DecodeMessageTypeList<Type>::type,
#include "fisch/vx/container.def"
    /// @endcond
    >>
    decode_message_types;

template <typename C>
constexpr static size_t decode_message_types_index = hate::index_type_list_by_type<
    typename DecodeMessageType<decltype(&C::decode)>::type,
    decode_message_types>::value;

} // namespace detail

} // namespace fisch::vx
