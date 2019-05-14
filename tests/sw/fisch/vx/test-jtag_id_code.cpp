#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"

TEST(JTAGIdCode, General)
{
	using namespace fisch::vx;

	EXPECT_EQ(JTAGIdCode::Value().value(), 0);

	EXPECT_NO_THROW(JTAGIdCode());

	JTAGIdCode config;
	EXPECT_EQ(config.get(), JTAGIdCode::Value());

	JTAGIdCode::Value id(0x12345678);
	hxcomm::vx::ut_message_from_fpga<hxcomm::vx::instruction::jtag_from_hicann::data> message(
	    hxcomm::vx::instruction::jtag_from_hicann::data::payload_type(id.value()));

	config.decode({message});
	EXPECT_EQ(config.get(), id);

	JTAGIdCode other_config = config;

	EXPECT_EQ(other_config, config);
	EXPECT_NE(JTAGIdCode(), config);
}

TEST(JTAGIdCode, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	JTAGIdCode obj;

	auto messages = obj.encode_read(typename JTAGIdCode::coordinate_type());

	EXPECT_EQ(messages.size(), 2);
	auto message_ins =
	    boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::ins>>(messages.at(0));
	EXPECT_EQ(message_ins.decode(), instruction::to_fpga_jtag::ins::IDCODE);
	auto message_data =
	    boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::data>>(messages.at(1));
	EXPECT_EQ(message_data.decode().get_payload(), 0);
	EXPECT_EQ(message_data.decode().get_keep_response(), true);
	EXPECT_EQ(message_data.decode().get_num_bits(), sizeof(uint32_t) * CHAR_BIT);
}

TEST(JTAGIdCode, Ostream)
{
	using namespace fisch::vx;

	JTAGIdCode obj;

	JTAGIdCode::Value id(0x12345678);
	hxcomm::vx::ut_message_from_fpga<hxcomm::vx::instruction::jtag_from_hicann::data> message(
	    hxcomm::vx::instruction::jtag_from_hicann::data::payload_type(id.value()));

	obj.decode({message});

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGIdCode(0x12345678)");
}

TEST(JTAGIdCode, CerealizeCoverage)
{
	using namespace fisch::vx;

	JTAGIdCode obj1, obj2;
	JTAGIdCode::Value id(0x12345678);
	hxcomm::vx::ut_message_from_fpga<hxcomm::vx::instruction::jtag_from_hicann::data> message(
	    hxcomm::vx::instruction::jtag_from_hicann::data::payload_type(id.value()));
	obj1.decode({message});

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
