#include <gtest/gtest.h>

#include "fisch/vx/jtag.h"
#include "fisch/vx/playback_executor.h"
#include "fisch/vx/reset.h"
#include "hxcomm/vx/simconnection.h"

extern std::string simulation_ip;
extern int simulation_port;

TEST(JTAGIdCode, Readout)
{
	fisch::vx::PlaybackProgramBuilder builder;

	fisch::vx::ResetChip reset;
	reset.set(true);
	builder.write<fisch::vx::ResetChip>(halco::hicann_dls::vx::ResetChipOnDLS(), reset);
	builder.write<fisch::vx::Timer>(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer());
	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(10));
	reset.set(false);
	builder.write<fisch::vx::ResetChip>(halco::hicann_dls::vx::ResetChipOnDLS(), reset);
	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(100));

	fisch::vx::JTAGClockScaler jtag_clock_scaler;
	jtag_clock_scaler.set(fisch::vx::JTAGClockScaler::Value(3));
	builder.write<fisch::vx::JTAGClockScaler>(
	    halco::hicann_dls::vx::JTAGOnDLS(), jtag_clock_scaler);
	builder.write<fisch::vx::ResetJTAGTap>(
	    halco::hicann_dls::vx::JTAGOnDLS(), fisch::vx::ResetJTAGTap());

	auto ticket = builder.read<fisch::vx::JTAGIdCode>(halco::hicann_dls::vx::JTAGOnDLS());

	builder.wait_until(halco::hicann_dls::vx::TimerOnDLS(), fisch::vx::Timer::Value(1000));
	builder.halt();
	auto program = builder.done();

	fisch::vx::PlaybackProgramExecutor<hxcomm::vx::SimConnection> executor(
	    simulation_ip, simulation_port);
	executor.transfer(program);
	executor.execute();
	executor.fetch(program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get().get(), 0x48580AF);
}
