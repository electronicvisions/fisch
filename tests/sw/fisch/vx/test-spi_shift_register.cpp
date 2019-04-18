#include <gtest/gtest.h>

#include "fisch/cerealization.h"
#include "fisch/vx/spi.h"

TEST(SPIShiftRegister, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(SPIShiftRegister());

	SPIShiftRegister default_config;
	EXPECT_EQ(default_config.get(), SPIShiftRegister::Value());

	SPIShiftRegister::Value value(0x345678);
	SPIShiftRegister value_config(value);
	EXPECT_EQ(value_config.get(), value);

	SPIShiftRegister::Value other_value(0x654321);
	value_config.set(other_value);
	EXPECT_EQ(value_config.get(), other_value);

	SPIShiftRegister other_config = value_config;

	EXPECT_EQ(other_config, value_config);
	EXPECT_NE(default_config, value_config);
}

TEST(SPIShiftRegister, Ostream)
{
	using namespace fisch::vx;

	SPIShiftRegister obj(SPIShiftRegister::Value(13));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SPIShiftRegister(0d13 0xd 0b000000000000000000001101)");
}

TEST(SPIShiftRegister, CerealizeCoverage)
{
	using namespace fisch::vx;

	SPIShiftRegister obj1, obj2;
	obj1.set(SPIShiftRegister::Value(0x345678));

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
