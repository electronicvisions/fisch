#include <gtest/gtest.h>

#include "fisch/vx/fill.h"

#include "fisch/vx/container.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	TEST(Name, IsRandomFillable)                                                                   \
	{                                                                                              \
		/* Change this seed if (by chance) the default value is hit for any container */           \
		std::mt19937 rng(1234);                                                                    \
		auto const config = fisch::vx::fill_random<Type>(rng);                                     \
		if constexpr (                                                                             \
		    !std::is_same<Type, fisch::vx::Timer>::value &&                                        \
		    fisch::vx::detail::HasValue<Type>::value) {                                            \
			EXPECT_NE(config, Type{});                                                             \
		}                                                                                          \
	}
#include "fisch/vx/container.def"

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	TEST(Name, IsOneFillable) { [[maybe_unused]] auto const config = fisch::vx::fill_ones<Type>(); }
#include "fisch/vx/container.def"
