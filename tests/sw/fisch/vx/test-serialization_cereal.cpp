#include <gtest/gtest.h>

#include "fisch/vx/container.h"

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include "halco/common/cerealization_geometry.h"

using namespace fisch::vx;

template <class T>
class CommonSerializationTests : public ::testing::Test
{};

using namespace fisch::vx;

#define PLAYBACK_CONTAINER(Name, Type) Type,
#define LAST_PLAYBACK_CONTAINER(Name, Type) Type
typedef ::testing::Types<
    SpikeLabel,
    SpikeFromChip,
    MADCSampleFromChip,
    SpikeFromChipEvent,
    MADCSampleFromChipEvent,
#include "fisch/vx/container.def"
    >
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
