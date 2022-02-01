#pragma once
#include "hxcomm/vx/utmessage_fwd.h"
#include <type_traits>
#include <vector>
#include <boost/range/sub_range.hpp>

namespace fisch::vx {

template <typename Instruction>
using UTMessageFromFPGARange =
    boost::sub_range<std::vector<hxcomm::vx::UTMessageFromFPGA<Instruction>> const>;


typedef UTMessageFromFPGARange<hxcomm::vx::instruction::jtag_from_hicann::Data>
    UTMessageFromFPGARangeJTAG;

typedef UTMessageFromFPGARange<hxcomm::vx::instruction::omnibus_from_fpga::Data>
    UTMessageFromFPGARangeOmnibus;

typedef UTMessageFromFPGARange<hxcomm::vx::instruction::from_fpga_system::Loopback>
    UTMessageFromFPGARangeLoopback;

namespace detail {

template <typename T>
struct SubRangeValueType
{
	typedef void type;
};

template <typename T>
struct SubRangeValueType<boost::sub_range<T>>
{
	typedef T type;
};

template <typename F>
struct DecodeMessageType;

template <typename C, typename M>
struct DecodeMessageType<void (C::*)(M const&)>
{
	typedef typename SubRangeValueType<M>::type::value_type type;
};

template <typename C, typename = void>
struct DecodeMessageTypeList
{
	typedef hate::type_list<> type;
};

template <typename C>
struct DecodeMessageTypeList<C, std::void_t<decltype(&C::decode)>>
{
	typedef hate::type_list<typename DecodeMessageType<decltype(&C::decode)>::type> type;
};

} // namespace detail

} // namespace fisch::vx
