#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/timer.h"

#include "executor.h"

TEST(OmnibusChipOverJTAG, SystimeSyncBaseWriteRead)
{
	fisch::vx::PlaybackProgramBuilder builder;

	builder.write<fisch::vx::ResetChip>(
	    halco::hicann_dls::vx::ResetChipOnDLS(), fisch::vx::ResetChip(true));
	builder.write<fisch::vx::Timer>(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer());
	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(10));
	builder.write<fisch::vx::ResetChip>(
	    halco::hicann_dls::vx::ResetChipOnDLS(), fisch::vx::ResetChip(false));
	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(100));

	builder.write<fisch::vx::JTAGClockScaler>(
	    halco::hicann_dls::vx::JTAGClockScalerOnDLS(), fisch::vx::JTAGClockScaler(fisch::vx::JTAGClockScaler::Value(3)));
	builder.write<fisch::vx::ResetJTAGTap>(
	    halco::hicann_dls::vx::ResetJTAGTapOnDLS(), fisch::vx::ResetJTAGTap());

	// wait until Omnibus is up (22 us)
	builder.wait_until(
	    halco::hicann_dls::vx::TimerOnDLS(),
	    fisch::vx::Timer::Value(22 * fisch::vx::fpga_clock_cycles_per_us));

	fisch::vx::OmnibusChipOverJTAG config;
	// random data
	config.set(fisch::vx::OmnibusData(10));

	// Omnibus address of SystimeSyncBase
	halco::hicann_dls::vx::OmnibusChipOverJTAGAddress addr(0x0);

	builder.write(addr, config);
	auto ticket = builder.read(addr);
	EXPECT_FALSE(ticket.valid());

	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(10000));
	auto program = builder.done();

	auto executor = generate_playback_program_test_executor();
	executor.run(program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get(), config);
}
