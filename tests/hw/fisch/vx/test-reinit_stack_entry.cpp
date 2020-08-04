#include <gtest/gtest.h>

#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reinit_stack_entry.h"
#include "fisch/vx/run.h"

#include "hxcomm/vx/connection_from_env.h"

using namespace fisch::vx;

TEST(ReinitStackEntry, Register)
{
	auto empty_program = PlaybackProgramBuilder().done();
	auto connection = hxcomm::vx::get_connection_from_env();

	ReinitStackEntry reinit1{connection};
	run(connection, empty_program);

	reinit1.set(empty_program);

	{
		ReinitStackEntry reinit2{connection};
		reinit2.set(empty_program);

		{
			ReinitStackEntry reinit3{connection};

			reinit3.set(empty_program);

			run(connection, empty_program);
		}
		run(connection, empty_program);
	}
	run(connection, empty_program);
}
