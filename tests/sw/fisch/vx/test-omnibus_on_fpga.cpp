#include <gtest/gtest.h>

#include "fisch/vx/omnibus.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include "fisch/vx/omnibus_constants.h"
#include "hxcomm/vx/utmessage.h"

template <typename T>
void test_omnibus_general()
{
	EXPECT_NO_THROW(T());

	T default_config;
	EXPECT_EQ(default_config.get(), typename T::value_type());

	typename T::value_type value(0x12345678);
	T value_config(value);
	EXPECT_EQ(value_config.get(), value);

	typename T::value_type other_value(0x87654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	T other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(OmnibusChip, General)
{
	using namespace fisch::vx;
	test_omnibus_general<OmnibusChip>();
}

TEST(OmnibusFPGA, General)
{
	using namespace fisch::vx;
	test_omnibus_general<OmnibusFPGA>();
}

template <typename T, uint32_t AddressMask>
void test_omnibus_encode_read()
{
	using namespace hxcomm::vx;

	T obj;

	typename T::coordinate_type coord(3);
	auto messages = obj.encode_read(coord);

	EXPECT_EQ(messages.size(), 1);
	auto message_addr =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	        instruction::omnibus_to_fpga::Address::Payload(AddressMask | coord, true)));
}

TEST(OmnibusChip, EncodeRead)
{
	using namespace fisch::vx;
	test_omnibus_encode_read<OmnibusChip, 0>();
}

TEST(OmnibusFPGA, EncodeRead)
{
	using namespace fisch::vx;
	test_omnibus_encode_read<OmnibusFPGA, static_cast<uint32_t>(executor_omnibus_mask)>();
}

template <typename T, uint32_t AddressMask>
void test_omnibus_encode_write()
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	T obj;
	obj.set(OmnibusData(12));

	typename T::coordinate_type coord(3);
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 2);
	auto message_addr =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	        instruction::omnibus_to_fpga::Address::Payload(AddressMask | coord, false)));
	auto message_data =
	    boost::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                      instruction::omnibus_to_fpga::Data::Payload(obj.get().value())));
}

TEST(OmnibusChip, EncodeWrite)
{
	using namespace fisch::vx;
	test_omnibus_encode_write<OmnibusChip, 0>();
}

TEST(OmnibusFPGA, EncodeWrite)
{
	using namespace fisch::vx;
	test_omnibus_encode_write<OmnibusFPGA, static_cast<uint32_t>(executor_omnibus_mask)>();
}

template <typename T>
void test_omnibus_decode()
{
	using namespace hxcomm::vx;

	T obj;

	UTMessageFromFPGA<instruction::omnibus_from_fpga::Data> message(
	    instruction::omnibus_from_fpga::Data::Payload(0x123));

	obj.decode({&message, &message + 1});
	EXPECT_EQ(obj.get(), 0x123);
}

TEST(OmnibusChip, Decode)
{
	using namespace fisch::vx;
	test_omnibus_decode<OmnibusChip>();
}

TEST(OmnibusFPGA, Decode)
{
	using namespace fisch::vx;
	test_omnibus_decode<OmnibusFPGA>();
}

TEST(OmnibusChip, Ostream)
{
	using namespace fisch::vx;

	OmnibusChip obj(OmnibusData(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "OmnibusChip(0d13 0xd 0b00000000000000000000000000001101)");
}

TEST(OmnibusFPGA, Ostream)
{
	using namespace fisch::vx;

	OmnibusFPGA obj(OmnibusData(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "OmnibusFPGA(0d13 0xd 0b00000000000000000000000000001101)");
}

template <typename T>
void test_cerealize_coverage()
{
	T obj1, obj2;
	obj1.set(typename T::value_type(0x12345678));

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}

TEST(OmnibusChip, CerealizeCoverage)
{
	using namespace fisch::vx;
	test_cerealize_coverage<OmnibusChip>();
}

TEST(OmnibusFPGA, CerealizeCoverage)
{
	using namespace fisch::vx;
	test_cerealize_coverage<OmnibusFPGA>();
}
