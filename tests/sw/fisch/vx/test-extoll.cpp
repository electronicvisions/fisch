#include <gtest/gtest.h>

#include "fisch/vx/extoll.h"

#include "fisch/vx/encode.h"
#include "fisch/vx/omnibus_constants.h"
#include "halco/hicann-dls/vx/extoll.h"
#include "hxcomm/vx/utmessage.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

TEST(Extoll, General)
{
	using namespace fisch::vx;
	EXPECT_NO_THROW(Extoll());

	Extoll default_config;
	EXPECT_EQ(default_config.get(), Extoll::Value());

	Extoll::Value value(0xbeef'cafe'babe'abba);
	Extoll value_config(value);
	EXPECT_EQ(value_config.get(), value);

	Extoll::Value other_value(0xbeef'beef'abba'abba);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	Extoll other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

std::array<halco::hicann_dls::vx::OmnibusAddress, 2> ExtollAddressToOmnibusAddress(
    halco::hicann_dls::vx::ExtollAddress const eaddr)
{
	auto const to_omnibus = [eaddr](size_t const offset) {
		auto omniAddr = halco::hicann_dls::vx::OmnibusAddress((eaddr.value() >> 2) + offset);
		if (omniAddr >= (fisch::vx::external_omnibus_bit_switch - 0x1 + offset)) {
			throw std::runtime_error(
			    "The given Extoll Registerfile Address (" + std::to_string(eaddr.value()) +
			    ") is too large for conversion into the current Omnibus Tree!");
		}
		return halco::hicann_dls::vx::OmnibusAddress(
		    fisch::vx::external_omnibus_base_address + omniAddr);
	};

	return {to_omnibus(0x0), to_omnibus(0x1)};
}

TEST(Extoll, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Extoll obj;

	Extoll::coordinate_type coord(3);
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_read(coord, emplacer);
	auto omni_addresses = ExtollAddressToOmnibusAddress(coord);

	EXPECT_EQ(messages.size(), 2);
	auto message_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                      instruction::omnibus_to_fpga::Address::Payload(omni_addresses[0], true)));
	message_addr = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(1));
	EXPECT_EQ(
	    message_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                      instruction::omnibus_to_fpga::Address::Payload(omni_addresses[1], true)));
}

TEST(Extoll, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Extoll obj;
	obj.set(Extoll::Value(0xcafe'0000'babe));

	Extoll::coordinate_type coord(3);
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(coord, emplacer);
	auto omni_addresses = ExtollAddressToOmnibusAddress(coord);

	EXPECT_EQ(messages.size(), 4);
	auto message_addr_0 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    message_addr_0, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                        instruction::omnibus_to_fpga::Address::Payload(omni_addresses[0], 0)));
	auto message_addr_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(
	    message_addr_1, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                        instruction::omnibus_to_fpga::Address::Payload(omni_addresses[1], 0)));
	auto message_data_0 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    message_data_0,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(obj.get().value() & 0xffff'ffff)));
	auto message_data_1 =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data_1,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload((obj.get().value() >> 32) & 0xffff'ffff)));
}

TEST(Extoll, Decode)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	Extoll obj;

	UTMessageFromFPGA<instruction::omnibus_from_fpga::Data> message_one(
	    instruction::omnibus_from_fpga::Data::Payload(0xcafe));
	UTMessageFromFPGA<instruction::omnibus_from_fpga::Data> message_two(
	    instruction::omnibus_from_fpga::Data::Payload(0xbeef));

	std::vector messages{message_one, message_two};
	obj.decode({messages.begin(), messages.end()});
	EXPECT_EQ(obj.get(), Extoll::Value(0xbeef'0000'cafe));
}

TEST(Extoll, Ostream)
{
	using namespace fisch::vx;

	Extoll obj(Extoll::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(
	    stream.str(),
	    "Extoll(0d13 0xd 0b0000000000000000000000000000000000000000000000000000000000001101)");
}

TEST(Extoll, CerealizeCoverage)
{
	using namespace fisch::vx;
	Extoll obj1, obj2;
	obj1.set(Extoll::Value(0x12345678));

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


TEST(ExtollOnNwNode, General)
{
	using namespace fisch::vx;
	EXPECT_NO_THROW(ExtollOnNwNode());

	ExtollOnNwNode default_config;
	EXPECT_EQ(default_config.get(), ExtollOnNwNode::Value());

	ExtollOnNwNode::Value value(0xbeef'cafe'babe'abba);
	ExtollOnNwNode value_config(value);
	EXPECT_EQ(value_config.get(), value);

	ExtollOnNwNode::Value other_value(0xbeef'beef'abba'abba);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	ExtollOnNwNode other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(ExtollOnNwNode, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;
	using namespace halco::hicann_dls::vx;

	ExtollOnNwNode obj;

	ExtollOnNwNode::coordinate_type coord(
	    ExtollAddress(3),
	    ExtollNodeIdOnExtollNetwork(ExtollNodeId(5), ExtollChipType(ExtollChipType::fpga)));
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_read(coord, emplacer);
	auto omni_addresses = ExtollAddressToOmnibusAddress(coord.toExtollAddress());

	EXPECT_EQ(messages.size(), 6);
	auto message_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(
	    message_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                      instruction::omnibus_to_fpga::Address::Payload(omni_addresses[0], true)));
	message_addr = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(3));
	EXPECT_EQ(
	    message_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                      instruction::omnibus_to_fpga::Address::Payload(omni_addresses[1], true)));

	auto ndid_conf = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    ndid_conf, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                   instruction::omnibus_to_fpga::Data::Payload(0x1'0005)));
	ndid_conf = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    ndid_conf, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                   instruction::omnibus_to_fpga::Data::Payload(0x0'0000)));

	auto conf_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    conf_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                   instruction::omnibus_to_fpga::Address::Payload(0x8d00'0000, false)));
	conf_addr = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(
	    conf_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                   instruction::omnibus_to_fpga::Address::Payload(0x8d00'0000, false)));
}

TEST(ExtollOnNwNode, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;
	using namespace halco::hicann_dls::vx;

	ExtollOnNwNode obj;
	obj.set(ExtollOnNwNode::Value(0xcafe'0000'babe));

	ExtollOnNwNode::coordinate_type coord(
	    ExtollAddress(3),
	    ExtollNodeIdOnExtollNetwork(ExtollNodeId(5), ExtollChipType(ExtollChipType::fpga)));
	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(coord, emplacer);
	auto omni_addresses = ExtollAddressToOmnibusAddress(coord.toExtollAddress());

	EXPECT_EQ(messages.size(), 8);
	auto message_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(2));
	EXPECT_EQ(
	    message_addr,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	        instruction::omnibus_to_fpga::Address::Payload(omni_addresses[0], false)));
	message_addr = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(4));
	EXPECT_EQ(
	    message_addr,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	        instruction::omnibus_to_fpga::Address::Payload(omni_addresses[1], false)));

	auto ndid_conf = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(1));
	EXPECT_EQ(
	    ndid_conf, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                   instruction::omnibus_to_fpga::Data::Payload(0x1'0005)));
	ndid_conf = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(7));
	EXPECT_EQ(
	    ndid_conf, UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	                   instruction::omnibus_to_fpga::Data::Payload(0x0'0000)));

	auto conf_addr =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(0));
	EXPECT_EQ(
	    conf_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                   instruction::omnibus_to_fpga::Address::Payload(0x8d00'0000, false)));
	conf_addr = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Address>>(messages.at(6));
	EXPECT_EQ(
	    conf_addr, UTMessageToFPGA<instruction::omnibus_to_fpga::Address>(
	                   instruction::omnibus_to_fpga::Address::Payload(0x8d00'0000, false)));

	auto message_data =
	    std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(3));
	EXPECT_EQ(
	    message_data,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload(obj.get().value() & 0xffff'ffff)));
	message_data = std::get<UTMessageToFPGA<instruction::omnibus_to_fpga::Data>>(messages.at(5));
	EXPECT_EQ(
	    message_data,
	    UTMessageToFPGA<instruction::omnibus_to_fpga::Data>(
	        instruction::omnibus_to_fpga::Data::Payload((obj.get().value() >> 32) & 0xffff'ffff)));
}

TEST(ExtollOnNwNode, Decode)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	ExtollOnNwNode obj;

	UTMessageFromFPGA<instruction::omnibus_from_fpga::Data> message_one(
	    instruction::omnibus_from_fpga::Data::Payload(0xcafe));
	UTMessageFromFPGA<instruction::omnibus_from_fpga::Data> message_two(
	    instruction::omnibus_from_fpga::Data::Payload(0xbeef));

	std::vector messages{message_one, message_two};
	obj.decode({messages.begin(), messages.end()});
	EXPECT_EQ(obj.get(), ExtollOnNwNode::Value(0xbeef'0000'cafe));
}

TEST(ExtollOnNwNode, Ostream)
{
	using namespace fisch::vx;

	ExtollOnNwNode obj(ExtollOnNwNode::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(
	    stream.str(), "ExtollOnNwNode(0d13 0xd "
	                  "0b0000000000000000000000000000000000000000000000000000000000001101)");
}

TEST(ExtollOnNwNode, CerealizeCoverage)
{
	using namespace fisch::vx;
	ExtollOnNwNode obj1, obj2;
	obj1.set(ExtollOnNwNode::Value(0x12345678));

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
