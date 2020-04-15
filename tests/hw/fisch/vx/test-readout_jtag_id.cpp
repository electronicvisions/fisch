#include <gtest/gtest.h>

#include "fisch/vx/container_ticket.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/run.h"
#include "fisch/vx/timer.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "connection.h"

using namespace fisch::vx;
using namespace halco::hicann_dls::vx;

TEST(JTAGIdCode, Readout)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(true));
	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10));
	builder.write(ResetChipOnDLS(), ResetChip(false));
	builder.wait_until(TimerOnDLS(), Timer::Value(100));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	auto ticket = builder.read(JTAGIdCodeOnDLS());

	builder.wait_until(TimerOnDLS(), Timer::Value(1000));
	auto program = builder.done();

	auto connection = generate_test_connection();
	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	auto jtag_id = ticket.get().at(0).get();
	EXPECT_TRUE((jtag_id == 0x048580AF) || (jtag_id == 0x248580AF));
}
