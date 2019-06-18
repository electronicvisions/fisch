#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

TEST(OmnibusChipOverJTAG, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(OmnibusChipOverJTAG());

	OmnibusChipOverJTAG default_config;
	EXPECT_EQ(default_config.get(), OmnibusChipOverJTAG::value_type());

	OmnibusChipOverJTAG::value_type value(0x12345678);
	OmnibusChipOverJTAG value_config(value);
	EXPECT_EQ(value_config.get(), value);

	OmnibusChipOverJTAG::value_type other_value(0x87654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	OmnibusChipOverJTAG other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(OmnibusChipOverJTAG, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	OmnibusChipOverJTAG obj;

	typename OmnibusChipOverJTAG::coordinate_type coord(3);
	auto messages = obj.encode_read(coord);

	EXPECT_EQ(messages.size(), 6);
	auto message_ins_1 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins_1.decode().value(), instruction::to_fpga_jtag::Ins::OMNIBUS_ADDRESS);
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
	EXPECT_EQ(message_data_1.decode().get_payload(), (1ull << 32) | coord.toEnum().value());
	EXPECT_EQ(message_data_1.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_1.decode().get_num_bits(), 33);
	auto message_ins_2 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(2));
	EXPECT_EQ(message_ins_2.decode().value(), instruction::to_fpga_jtag::Ins::OMNIBUS_REQUEST);
	auto message_data_2 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(3));
	EXPECT_EQ(message_data_2.decode().get_payload(), 0);
	EXPECT_EQ(message_data_2.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_2.decode().get_num_bits(), 3);
	auto message_ins_3 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(4));
	EXPECT_EQ(message_ins_3.decode().value(), instruction::to_fpga_jtag::Ins::OMNIBUS_DATA);
	auto message_data_3 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(5));
	EXPECT_EQ(message_data_3.decode().get_payload(), 0);
	EXPECT_EQ(message_data_3.decode().get_keep_response(), true);
	EXPECT_EQ(message_data_3.decode().get_num_bits(), sizeof(uint32_t) * CHAR_BIT);
}

TEST(OmnibusChipOverJTAG, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	OmnibusChipOverJTAG obj;
	obj.set(OmnibusData(12));

	typename OmnibusChipOverJTAG::coordinate_type coord(3);
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 6);
	auto message_ins_1 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins_1.decode().value(), instruction::to_fpga_jtag::Ins::OMNIBUS_ADDRESS);
	auto message_data_1 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
	EXPECT_EQ(message_data_1.decode().get_payload(), coord.toEnum().value());
	EXPECT_EQ(message_data_1.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_1.decode().get_num_bits(), 33);
	auto message_ins_2 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(2));
	EXPECT_EQ(message_ins_2.decode().value(), instruction::to_fpga_jtag::Ins::OMNIBUS_DATA);
	auto message_data_2 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(3));
	EXPECT_EQ(message_data_2.decode().get_payload(), obj.get().value());
	EXPECT_EQ(message_data_2.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_2.decode().get_num_bits(), sizeof(uint32_t) * CHAR_BIT);
	auto message_ins_3 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(4));
	EXPECT_EQ(message_ins_3.decode().value(), instruction::to_fpga_jtag::Ins::OMNIBUS_REQUEST);
	auto message_data_3 =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(5));
	EXPECT_EQ(message_data_3.decode().get_payload(), 0);
	EXPECT_EQ(message_data_3.decode().get_keep_response(), false);
	EXPECT_EQ(message_data_3.decode().get_num_bits(), 3);
}

TEST(OmnibusChipOverJTAG, Decode)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	OmnibusChipOverJTAG obj;

	UTMessageFromFPGA<instruction::jtag_from_hicann::Data> message(
	    instruction::jtag_from_hicann::Data::Payload(0x123));

	std::array<UTMessageFromFPGAVariant, 1> messages{message};

	obj.decode(messages);
	EXPECT_EQ(obj.get(), 0x123);
}

TEST(OmnibusChipOverJTAG, Ostream)
{
	using namespace fisch::vx;

	OmnibusChipOverJTAG obj(OmnibusData(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "OmnibusChipOverJTAG(0d13 0xd 0b00000000000000000000000000001101)");
}

TEST(OmnibusChipOverJTAG, CerealizeCoverage)
{
	using namespace fisch::vx;

	OmnibusChipOverJTAG obj1, obj2;
	obj1.set(OmnibusChipOverJTAG::value_type(0x12345678));

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
