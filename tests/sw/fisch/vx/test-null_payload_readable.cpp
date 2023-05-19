#include <gtest/gtest.h>

#include "fisch/vx/null_payload_readable.h"

#include "fisch/cerealization.tcc"
#include "halco/hicann-dls/vx/fpga.h"
#include "hxcomm/vx/utmessage.h"

TEST(NullPayloadReadable, General)
{
	using namespace fisch::vx;

	EXPECT_NO_THROW(NullPayloadReadable());

	NullPayloadReadable config;
	NullPayloadReadable other_config = config;

	EXPECT_TRUE(other_config == config);
	EXPECT_FALSE(other_config != config);
}

TEST(NullPayloadReadable, EncodeRead)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	NullPayloadReadable obj;

	auto messages = obj.encode_read(typename NullPayloadReadable::coordinate_type());

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::system::Loopback>>(messages.at(0));
	EXPECT_EQ(message.decode(), instruction::system::Loopback::tick);
}

TEST(NullPayloadReadable, Ostream)
{
	using namespace fisch::vx;

	NullPayloadReadable obj;

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "NullPayloadReadable()");
}

TEST(NullPayloadReadable, CerealizeCoverage)
{
	using namespace fisch::vx;

	NullPayloadReadable obj1, obj2;

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
