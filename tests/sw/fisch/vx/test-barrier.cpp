#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/barrier.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_RANGED_REGISTER_GENERAL(Barrier, 0b00000, 32, 5)

TEST(Barrier, Ostream)
{
	Barrier obj;
	obj.set(Barrier::Value(5));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "Barrier(0d5 0x5 0b00101)");
}

TEST(Barrier, EncodeWrite)
{
	using namespace hxcomm::vx;

	Barrier obj;
	obj.set(Barrier::Value(5));

	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(typename Barrier::coordinate_type(), emplacer);

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::timing::Barrier>>(messages.at(0));
	EXPECT_EQ(static_cast<uintmax_t>(message.decode()), obj.get());
}

FISCH_TEST_NUMBER_REGISTER_CEREAL(Barrier, 5)
