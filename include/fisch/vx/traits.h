#pragma once

namespace fisch::vx {

/**
 * Check whether given container is readable.
 * @tparam ContainerT Type of container
 */
template <typename ContainerT>
struct IsReadable
{
	constexpr static bool value = (ContainerT::encode_read_ut_message_count > 0);
};


/**
 * Check whether given container is writable.
 * @tparam ContainerT Type of container
 */
template <typename ContainerT>
struct IsWritable
{
	constexpr static bool value = (ContainerT::encode_write_ut_message_count > 0);
};

} // namespace fisch::vx
