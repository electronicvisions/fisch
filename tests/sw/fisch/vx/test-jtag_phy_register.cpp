#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(JTAGPhyRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(JTAGPhyRegister::Value());
	EXPECT_EQ(JTAGPhyRegister::Value().value(), 0);
	EXPECT_THROW(JTAGPhyRegister::Value(4194304), std::overflow_error);

	EXPECT_NO_THROW(JTAGPhyRegister());

	JTAGPhyRegister config;
	EXPECT_EQ(config.get(), JTAGPhyRegister::Value());

	JTAGPhyRegister::Value const value(12);
	config.set(value);
	EXPECT_EQ(config.get(), value);

	JTAGPhyRegister config2(value);
	EXPECT_EQ(config2.get(), value);

	JTAGPhyRegister other_config = config;

	EXPECT_EQ(other_config, config);
	EXPECT_NE(JTAGPhyRegister(), config);
}

TEST(JTAGPhyRegister, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	JTAGPhyRegister obj;
	obj.set(JTAGPhyRegister::Value(12));

	typename JTAGPhyRegister::coordinate_type coord(3);
	auto messages = obj.encode_write(coord);

	EXPECT_EQ(messages.size(), 2);
	auto message_ins = boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Ins>>(messages.at(0));
	EXPECT_EQ(message_ins.decode().value(), 92);
	auto message_data =
	    boost::get<UTMessageToFPGA<instruction::to_fpga_jtag::Data>>(messages.at(1));
	EXPECT_EQ(message_data.decode().get_payload(), 12);
	EXPECT_EQ(message_data.decode().get_keep_response(), false);
	EXPECT_EQ(message_data.decode().get_num_bits(), 22);
}

TEST(JTAGPhyRegister, Ostream)
{
	using namespace fisch::vx;

	JTAGPhyRegister obj;
	obj.set(JTAGPhyRegister::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGPhyRegister(0d12 0xc 0b0000000000000000001100)");
}

TEST(JTAGPhyRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	JTAGPhyRegister obj1, obj2;
	obj1.set(JTAGPhyRegister::Value(12));

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
