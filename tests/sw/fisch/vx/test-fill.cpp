#include <gtest/gtest.h>

#include "fisch/vx/fill.h"

#include "fisch/vx/container.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	TEST(Name, IsRandomFillable)                                                                   \
	{                                                                                              \
		/* Change this seed if (by chance) the default value is hit for any container */           \
		constexpr size_t num_tries = 2;                                                            \
		std::mt19937 rng(1234);                                                                    \
		bool is_non_default = false;                                                               \
		Type default_config;                                                                       \
		for (size_t i = 0; i < num_tries; ++i) {                                                   \
			auto const config = fisch::vx::fill_random<Type>(rng);                                 \
			is_non_default = is_non_default || config != default_config;                           \
		}                                                                                          \
		if constexpr (                                                                             \
		    !std::is_same<Type, fisch::vx::Timer>::value &&                                        \
		    fisch::vx::detail::HasValue<Type>::value) {                                            \
			EXPECT_TRUE(is_non_default);                                                           \
		}                                                                                          \
	}
#include "fisch/vx/container.def"

#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	TEST(Name, IsOneFillable) { [[maybe_unused]] auto const config = fisch::vx::fill_ones<Type>(); }
#include "fisch/vx/container.def"
