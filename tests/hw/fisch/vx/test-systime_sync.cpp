#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/systime.h"
#include "fisch/vx/timer.h"

#include "executor.h"

using namespace halco::common;
using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

// TODO: move to haldls in order to get sane init routine (Issue #3220)
TEST(SystimeSync, TimeAnnotation)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(true));
	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10));
	builder.write(ResetChipOnDLS(), ResetChip(false));
	builder.wait_until(TimerOnDLS(), Timer::Value(100));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// configure FPGA-side PHYs
	for (auto i : iter_all<PhyConfigFPGAOnDLS>()) {
		OmnibusFPGA config(OmnibusData(0x0020'4040));
		OmnibusFPGAAddress coord(0x0600'0000 + i);
		builder.write(coord, config);
	}

	// enable FPGA-side PHYs
	{
		OmnibusFPGA config(OmnibusData(0xff));
		OmnibusFPGAAddress coord(0x0400'0000);
		builder.write(coord, config);
	}

	// configure Chip-side PHYs
	for (auto coord : iter_all<JTAGPhyRegisterOnDLS>()) {
		JTAGPhyRegister config(JTAGPhyRegister::Value(0x0020'4000));
		builder.write(coord, config);
	}

	// wait until Omnibus is up
	builder.wait_until(TimerOnDLS(), Timer::Value(22 * fpga_clock_cycles_per_us));

	// enable Chip-side PHYs
	{
		OmnibusChipOverJTAG config(OmnibusData(0xff));
		OmnibusChipOverJTAGAddress coord(0x0004'0000);
		builder.write(coord, config);
	}

	// wait until highspeed is up
	builder.wait_until(TimerOnDLS(), Timer::Value(80 * fpga_clock_cycles_per_us));

	builder.write(SystimeSyncOnFPGA(), SystimeSync(true));

	// wait until systime init is finished
	builder.wait_until(TimerOnDLS(), Timer::Value(85 * fpga_clock_cycles_per_us));

	OmnibusChipOverJTAG config;
	// random data
	config.set(OmnibusData(10));

	// Omnibus address of first PPU memory word of top PPU
	OmnibusChipOverJTAGAddress addr(0x0);

	builder.write(addr, config);
	auto ticket = builder.read(addr);
	EXPECT_FALSE(ticket.valid());

	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10000));
	auto program = builder.done();

	auto executor = generate_playback_program_test_executor();
	executor.run(program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get(), config);
	// JTAG is slow, 1 is expected, if no systime init happened
	EXPECT_TRUE(ticket.fpga_time() > 1);
}
