#include <gtest/gtest.h>

#include "fisch/vx/reset.h"

#include "fisch/vx/encode.h"
#include "halco/hicann-dls/vx/reset.h"
#include "hxcomm/vx/utmessage.h"
#include "test-macros.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

using namespace fisch::vx;

FISCH_TEST_BOOLEAN_REGISTER_GENERAL(ResetChip)

FISCH_TEST_BOOLEAN_REGISTER_OSTREAM(ResetChip, ResetChip)

FISCH_TEST_BOOLEAN_REGISTER_CEREAL(ResetChip)

TEST(ResetChip, EncodeWrite)
{
	using namespace fisch::vx;
	using namespace hxcomm::vx;

	ResetChip::Value value(true);
	ResetChip obj(value);

	std::vector<UTMessageToFPGAVariant> messages;
	UTMessageToFPGABackEmplacer emplacer(messages);
	obj.encode_write(typename ResetChip::coordinate_type(), emplacer);

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::system::Reset>>(messages.at(0));
	EXPECT_EQ(message.decode(), value.value());
}
