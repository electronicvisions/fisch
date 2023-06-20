#pragma once
#include "fisch/vx/encode_fwd.h"
#include "hate/type_traits.h"
#include <type_traits>
#include <vector>
#include <boost/utility/enable_if.hpp>

namespace fisch::vx {

namespace detail {

template <typename T, typename = void>
struct HasPositiveDecodeUTMessageCount : public std::false_type
{};

template <typename T>
struct HasPositiveDecodeUTMessageCount<
    T,
    typename boost::enable_if_has_type<decltype(T::decode_ut_message_count)>::type>
{
	constexpr static bool value = (T::decode_ut_message_count > 0);
};

template <typename T>
using HasEncodeRead = decltype(T::encode_read(
    std::declval<typename T::coordinate_type const&>(),
    std::declval<UTMessageToFPGABackEmplacer&>()));

template <typename T>
using HasEncodeWrite = decltype(std::declval<T const>().encode_write(
    std::declval<typename T::coordinate_type const&>(),
    std::declval<UTMessageToFPGABackEmplacer&>()));

} // namespace detail


/**
 * Check whether given container is readable.
 * @tparam ContainerT Type of container
 */
template <typename ContainerT>
struct IsReadable
{
	constexpr static bool value = detail::HasPositiveDecodeUTMessageCount<ContainerT>::value &&
	                              hate::is_detected_v<detail::HasEncodeRead, ContainerT>;
};


/**
 * Check whether given container is writable.
 * @tparam ContainerT Type of container
 */
template <typename ContainerT>
struct IsWritable
{
	constexpr static bool value = hate::is_detected_v<detail::HasEncodeWrite, ContainerT>;
};


namespace detail {

template <typename T, typename = void>
struct HasValue : public std::false_type
{};

template <typename T>
struct HasValue<T, typename boost::enable_if_has_type<typename T::Value>::type>
    : public std::true_type
{};

} // namespace detail

} // namespace fisch::vx
