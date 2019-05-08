#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"
#include "fisch/vx/reset.h"

#include "executor.h"

TEST(JTAGIdCode, Readout)
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

	auto ticket = builder.read<fisch::vx::JTAGIdCode>(halco::hicann_dls::vx::JTAGIdCodeOnDLS());

	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(1000));
	builder.halt();
	auto program = builder.done();

	auto executor = generate_playback_program_test_executor();
	executor.run(program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get().get(), 0x48580AF);
}
