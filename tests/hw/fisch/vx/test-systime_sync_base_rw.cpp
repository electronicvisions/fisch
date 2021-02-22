#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/constants.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/run.h"
#include "fisch/vx/timer.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/connection_from_env.h"

using namespace fisch::vx;
using namespace halco::hicann_dls::vx;

TEST(OmnibusChipOverJTAG, SystimeSyncBaseWriteRead)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(true));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(false));
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100)));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// wait until Omnibus is up (22 us)
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(22 * fpga_clock_cycles_per_us)));

	OmnibusChipOverJTAG config;
	// random data
	config.set(OmnibusData(10));

	// Omnibus address of SystimeSyncBase
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
}
