#include <gtest/gtest.h>

#include "fisch/vx/event.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/spi.h"
#include "fisch/vx/systime.h"
#include "fisch/vx/timer.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

using namespace fisch::vx;

template <class T>
class CommonSerializationTests : public ::testing::Test
{};

typedef ::testing::Types<
    OmnibusChipOverJTAG,
    OmnibusChip,
    OmnibusFPGA,
    JTAGIdCode,
    JTAGPLLRegister,
    JTAGPhyRegister,
    ResetChip,
    ResetJTAGTap,
    JTAGClockScaler,
    SPIShiftRegister,
    Timer,
    SystimeSync,
    SpikePack1ToChip,
    SpikePack2ToChip,
    SpikePack3ToChip,
    SpikeLabel,
    SpikeFromChip,
    MADCSampleFromChip,
    SpikeFromChipEvent,
    MADCSampleFromChipEvent>
    SerializableTypes;

TYPED_TEST_CASE(CommonSerializationTests, SerializableTypes);

TYPED_TEST(CommonSerializationTests, IsDefaultConstructible)
{
	TypeParam obj;
	static_cast<void>(&obj);
}

TYPED_TEST(CommonSerializationTests, IsAssignable)
{
	TypeParam obj1, obj2;
	obj1 = obj2;
}

TYPED_TEST(CommonSerializationTests, HasSerialization)
{
	TypeParam obj1, obj2;

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
	// This does only test that Serialization does not insert wrong values
	// but does not check coverage since both instances are default constructed.
	// Coverage check is done in each container's test file.
	ASSERT_EQ(obj2, obj1);
}
