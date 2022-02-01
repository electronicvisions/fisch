#pragma once
#include "hate/type_list.h"
#include <type_traits>
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

template <typename T, typename = void>
struct HasPositiveEncodeReadUTMessageCount : public std::false_type
{};

template <typename T>
struct HasPositiveEncodeReadUTMessageCount<
    T,
    typename boost::enable_if_has_type<decltype(T::encode_read_ut_message_count)>::type>
{
	constexpr static bool value = (T::encode_read_ut_message_count > 0);
};

template <typename T, typename = void>
struct HasPositiveEncodeWriteUTMessageCount : public std::false_type
{};

template <typename T>
struct HasPositiveEncodeWriteUTMessageCount<
    T,
    typename boost::enable_if_has_type<decltype(T::encode_write_ut_message_count)>::type>
{
	constexpr static bool value = (T::encode_write_ut_message_count > 0);
};

} // namespace detail


/**
 * Check whether given container is readable.
 * @tparam ContainerT Type of container
 */
template <typename ContainerT>
struct IsReadable
{
	constexpr static bool value = detail::HasPositiveDecodeUTMessageCount<ContainerT>::value &&
	                              detail::HasPositiveEncodeReadUTMessageCount<ContainerT>::value;
};


/**
 * Check whether given container is writable.
 * @tparam ContainerT Type of container
 */
template <typename ContainerT>
struct IsWritable
{
	constexpr static bool value = detail::HasPositiveEncodeWriteUTMessageCount<ContainerT>::value;
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
