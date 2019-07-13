#include <gtest/gtest.h>

#include "fisch/vx/playback_program.h"
#include "fisch/vx/traits.h"

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
	using namespace fisch::vx;

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	if constexpr (IsReadable<Type>::value) {                                                       \
		EXPECT_NO_THROW(test_playback_program_builder_read_api<Type>());                           \
	} else {                                                                                       \
		EXPECT_THROW(test_playback_program_builder_read_api<Type>(), std::logic_error);            \
	}

#include "fisch/vx/container.def"
}

template <typename ContainerT>
void test_playback_program_builder_write_api()
{
	using namespace fisch::vx;

	PlaybackProgramBuilder builder;

	ContainerT config;
	std::vector<ContainerT> vector_config{config};

	typename ContainerT::coordinate_type coord;
	std::vector<typename ContainerT::coordinate_type> vector_coord{coord};

	// single write
	builder.write(coord, config);

	// vector write
	builder.write(vector_coord, vector_config);
}

TEST(PlaybackProgramBuilder, WriteAPI)
{
	using namespace fisch::vx;

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	if constexpr (IsWritable<Type>::value) {                                                       \
		EXPECT_NO_THROW(test_playback_program_builder_write_api<Type>());                          \
	} else {                                                                                       \
		EXPECT_THROW(test_playback_program_builder_write_api<Type>(), std::logic_error);           \
	}

#include "fisch/vx/container.def"
}
