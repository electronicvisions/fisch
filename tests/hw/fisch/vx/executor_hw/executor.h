#pragma once
#include "fisch/vx/playback_executor.h"

#define FISCH_TEST_ARQ_EXECUTOR
typedef fisch::vx::PlaybackProgramExecutor<hxcomm::vx::ARQConnection> PlaybackProgramTestExecutor;

PlaybackProgramTestExecutor generate_playback_program_test_executor();
