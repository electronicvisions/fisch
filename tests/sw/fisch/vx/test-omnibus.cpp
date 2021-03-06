#include <gtest/gtest.h>

#include "fisch/vx/omnibus.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include "fisch/vx/omnibus_constants.h"
#include "hxcomm/vx/utmessage.h"

TEST(Omnibus, General)
{
	using namespace fisch::vx;
	EXPECT_NO_THROW(Omnibus());

	Omnibus default_config;
	EXPECT_EQ(default_config.get(), Omnibus::Value());

	Omnibus::Value value(0x12345678);
	Omnibus::ByteEnables byte_enables = {true};
	Omnibus value_config(value, byte_enables);
	EXPECT_EQ(value_config.get(), value);

	Omnibus::Value other_value(0x87654321);
	Omnibus::ByteEnables other_byte_enables = {false};
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);
	value_config.set_byte_enables(other_byte_enables);
	EXPECT_EQ(value_config.get_byte_enables(), other_byte_enables);

	Omnibus other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(Omnibus, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Omnibus obj;

	Omnibus::coordinate_type coord(3);
	auto messages = obj.encode_read(coord);

	EXPECT_EQ(messages.size(), 1);
	auto message_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                      instruction::omnibus_to_fpga::Address::Payload(coord, true)));
}

TEST(Omnibus, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Omnibus obj;
	obj.set(Omnibus::Value(12));
	obj.set_byte_enables({false});

	Omnibus::coordinate_type coord(3);
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 2);
	auto message_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                      instruction::omnibus_to_fpga::Address::Payload(coord, false, 0)));
	auto message_data =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                      instruction::omnibus_to_fpga::Data::Payload(obj.get().value())));
}

TEST(Omnibus, Decode)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Omnibus obj;

	UTMessageFromFPGA<instruction::omnibus_from_fpga::Data> message(
	    instruction::omnibus_from_fpga::Data::Payload(0x123));

	obj.decode({&message, &message + 1});
	EXPECT_EQ(obj.get(), 0x123);
	EXPECT_EQ(obj.get_byte_enables(), Omnibus::ByteEnables({true, true, true, true}));
}

TEST(Omnibus, Ostream)
{
	using namespace fisch::vx;

	Omnibus obj(Omnibus::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(
	    stream.str(), "Omnibus(0d13 0xd 0b00000000000000000000000000001101, byte_enables: 1111)");
}

TEST(Omnibus, CerealizeCoverage)
{
	using namespace fisch::vx;
	Omnibus obj1, obj2;
	obj1.set(Omnibus::Value(0x12345678));

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
