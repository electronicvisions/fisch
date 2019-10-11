#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/timer.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "executor.h"

using namespace fisch::vx;
using namespace halco::hicann_dls::vx;

TEST(OmnibusChipOverJTAG, SystimeSyncBaseWriteRead)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(true));
	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10));
	builder.write(ResetChipOnDLS(), ResetChip(false));
	builder.wait_until(TimerOnDLS(), Timer::Value(100));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// wait until Omnibus is up (22 us)
	builder.wait_until(TimerOnDLS(), Timer::Value(22 * fpga_clock_cycles_per_us));

	OmnibusChipOverJTAG config;
	// random data
	config.set(OmnibusData(10));

	// Omnibus address of SystimeSyncBase
	OmnibusChipOverJTAGAddress addr(0x0);

	builder.write(addr, config);
	auto ticket = builder.read(addr);
	EXPECT_FALSE(ticket.valid());

	builder.wait_until(TimerOnDLS(), Timer::Value(10000));
	auto program = builder.done();

	auto executor = generate_playback_program_test_executor();
	executor.run(program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get(), config);
}
