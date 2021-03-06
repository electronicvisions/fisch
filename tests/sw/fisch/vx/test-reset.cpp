#include <gtest/gtest.h>

#include "fisch/vx/reset.h"

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

	bool value = true;
	ResetChip obj(value);

	auto messages = obj.encode_write(typename ResetChip::coordinate_type());

	EXPECT_EQ(messages.size(), 1);
	auto message = std::get<UTMessageToFPGA<instruction::system::Reset>>(messages.at(0));
	EXPECT_EQ(message.decode(), value);
}
