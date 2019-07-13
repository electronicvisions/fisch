#pragma once

namespace fisch::vx {

template <typename ContainerT>
struct IsReadable
{
	constexpr static bool value = (ContainerT::encode_read_ut_message_count > 0);
};


template <typename ContainerT>
struct IsWritable
{
	constexpr static bool value = (ContainerT::encode_write_ut_message_count > 0);
};

} // namespace fisch::vx
