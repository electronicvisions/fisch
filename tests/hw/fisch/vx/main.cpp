#include <string>

#include <gtest/gtest.h>

#include "executor.h"

PlaybackProgramTestExecutor generate_playback_program_test_executor()
{
	return PlaybackProgramTestExecutor();
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
