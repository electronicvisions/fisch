#pragma once
#include <random>
#include <type_traits>
#include <boost/utility/enable_if.hpp>

#include "fisch/vx/event.h"
#include "fisch/vx/timer.h"
#include "fisch/vx/traits.h"

namespace fisch::vx {

namespace detail {

template <typename T, typename = void>
struct IsRangedType : public std::false_type
{};

template <typename T>
struct IsRangedType<T, typename boost::enable_if_has_type<decltype(T::min)>::type>
    : public std::true_type
{};

} // namespace detail


/**
 * Draw a type T variable out of all values.
 * @tparam T type
 * @param gen RNG
 * @return random value
 */
template <typename T>
inline T fill_random(std::mt19937& gen)
{
	if constexpr (detail::HasValue<T>::value) {
		if constexpr (std::is_same_v<typename T::Value::value_type, bool>) {
			std::bernoulli_distribution d;
			return T(typename T::Value(d(gen)));
		} else if constexpr (detail::IsRangedType<typename T::Value>::value) {
			std::uniform_int_distribution<uintmax_t> d(T::Value::min, T::Value::max);
			return T(typename T::Value(d(gen)));
		} else {
			std::uniform_int_distribution<uintmax_t> d;
			return T(typename T::Value(d(gen)));
		}
	} else {
		return T();
	}
}


/**
 * Fill a type T variable with all bits set to one.
 * @tparam T Type
 * @return Value filled with ones
 */
template <typename T>
inline T fill_ones()
{
	if constexpr (detail::HasValue<T>::value) {
		if constexpr (detail::IsRangedType<typename T::Value>::value) {
			return T(typename T::Value(T::Value::max));
		} else {
			if constexpr (std::is_same<typename T::Value, bool>::value) {
				return T(true);
			} else {
				return T(typename T::Value(~typename T::Value::value_type(0)));
			}
		}
	} else {
		return T();
	}
}


// specializations


template <>
inline Timer fill_random(std::mt19937&)
{
	return Timer();
}

template <>
inline Timer fill_ones()
{
	return Timer();
}

#define SPIKE_PACK_TO_CHIP(Num)                                                                    \
	template <>                                                                                    \
	inline SpikePack##Num##ToChip fill_random(std::mt19937& gen)                                          \
	{                                                                                              \
		SpikePack##Num##ToChip spike_pack;                                                         \
		auto labels = spike_pack.get_labels();                                                     \
                                                                                                   \
		for (auto& label : labels) {                                                               \
			std::uniform_int_distribution<SpikeLabel::value_type> d;                               \
			label = SpikeLabel(d(gen));                                                            \
		}                                                                                          \
		spike_pack.set_labels(labels);                                                             \
		return spike_pack;                                                                         \
	}                                                                                              \
                                                                                                   \
	template <>                                                                                    \
	inline SpikePack##Num##ToChip fill_ones()                                                             \
	{                                                                                              \
		SpikePack##Num##ToChip spike_pack;                                                         \
		auto labels = spike_pack.get_labels();                                                     \
                                                                                                   \
		for (auto& label : labels) {                                                               \
			label = SpikeLabel(~SpikeLabel::value_type(0));                                        \
		}                                                                                          \
		spike_pack.set_labels(labels);                                                             \
		return spike_pack;                                                                         \
	}
SPIKE_PACK_TO_CHIP(1)
SPIKE_PACK_TO_CHIP(2)
SPIKE_PACK_TO_CHIP(3)
#undef SPIKE_PACK_TO_CHIP
} // namespace fisch::vx
