#include <gtest/gtest.h>

#include "fisch/vx/playback_program.h"

template <typename ContainerT>
void test_playback_program_builder_read_api()
{
	using namespace fisch::vx;

	PlaybackProgramBuilder builder;

	typename ContainerT::coordinate_type coord;
	std::vector<typename ContainerT::coordinate_type> vector_coord{coord};

	// single coordinate read
	builder.read(coord);

	// vector coordinate read
	builder.read(vector_coord);
}

TEST(PlaybackProgramBuilder, ReadAPI)
{
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	if constexpr (Type::encode_read_ut_message_count != 0 /* =: readable */) {                     \
		EXPECT_NO_THROW(test_playback_program_builder_read_api<Type>());                           \
	} else {                                                                                       \
		EXPECT_THROW(test_playback_program_builder_read_api<Type>(), std::logic_error);            \
	}

#include "fisch/vx/container.def"
}
