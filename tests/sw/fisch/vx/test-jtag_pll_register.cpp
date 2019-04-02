#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/jtag.h"

TEST(JTAGPLLRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(JTAGPLLRegister());

	JTAGPLLRegister config;

	JTAGPLLRegister config2;
	config2.set(JTAGPLLRegister::Value());
	EXPECT_EQ(config, config2);

	JTAGPLLRegister::Value data(0x12345678);
	JTAGPLLRegister config3;
	config3.set(data);
	EXPECT_NE(config, config3);

	JTAGPLLRegister config4(data);
	EXPECT_NE(config, config4);
	EXPECT_EQ(config3, config4);

	auto encoded = config3.encode_write(JTAGPLLRegister::coordinate_type());
	EXPECT_EQ(encoded.size(), JTAGPLLRegister::encode_write_ut_message_count);

	using namespace hxcomm::vx;
	auto payload = boost::get<ut_message_to_fpga<instruction::to_fpga_jtag::data>>(encoded.back())
	                   .decode()
	                   .get_payload();
	EXPECT_EQ(payload, data.value());
}

TEST(JTAGPLLRegister, Ostream)
{
	using namespace fisch::vx;

	JTAGPLLRegister obj;
	obj.set(JTAGPLLRegister::Value(12));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "JTAGPLLRegister(0d12 0xc 0b00000000000000000000000000001100)");
}

TEST(JTAGPLLRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	JTAGPLLRegister obj1, obj2;
	obj1.set(JTAGPLLRegister::Value(0x12345678));

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
