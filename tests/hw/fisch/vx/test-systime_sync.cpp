#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/run.h"
#include "fisch/vx/systime.h"
#include "fisch/vx/timer.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/connection_from_env.h"

using namespace halco::common;
using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

// TODO: move to haldls in order to get sane init routine (Issue #3220)
TEST(SystimeSync, TimeAnnotation)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(true)));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(false)));
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100)));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// configure FPGA-side PHYs
	for (auto i : iter_all<PhyConfigFPGAOnDLS>()) {
		Omnibus config(Omnibus::Value(0x0020'4040));
		OmnibusAddress coord{phy_omnibus_mask + i};
		builder.write(coord, config);
	}

	// enable FPGA-side PHYs
	{
		Omnibus config(Omnibus::Value(0xff));
		builder.write(ut_omnibus_mask, config);
	}

	// configure Chip-side PHYs
	for (auto coord : iter_all<JTAGPhyRegisterOnDLS>()) {
		JTAGPhyRegister config(JTAGPhyRegister::Value(0x0020'4000));
		builder.write(coord, config);
	}

	// wait until Omnibus is up
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(22 * fpga_clock_cycles_per_us)));

	// enable Chip-side PHYs
	{
		OmnibusChipOverJTAG config(OmnibusChipOverJTAG::Value(0xff));
		OmnibusChipOverJTAGAddress coord(0x0004'0000);
		builder.write(coord, config);
	}

	// wait until highspeed is up
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(80 * fpga_clock_cycles_per_us)));

	builder.write(SystimeSyncOnFPGA(), SystimeSync(SystimeSync::Value(true)));

	// wait until systime init is finished
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::systime));

	OmnibusChipOverJTAG config;
	// random data
	config.set(OmnibusChipOverJTAG::Value(10));

	// Omnibus address of first PPU memory word of top PPU
	OmnibusChipOverJTAGAddress addr(0x0);

	builder.write(addr, config);
	auto ticket = builder.read(addr);
	EXPECT_FALSE(ticket.valid());

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::jtag));
	auto program = builder.done();

	auto connection = hxcomm::vx::get_connection_from_env();
	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get().at(0), config);
	// JTAG is slow, 1 is expected, if no systime init happened
	EXPECT_TRUE(ticket.fpga_time() > 1);
}
