#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/timer.h"

#include "executor.h"

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

std::shared_ptr<PlaybackProgram> get_write_program(
    OmnibusChipOverJTAGAddress address, OmnibusChipOverJTAG config)
{
	PlaybackProgramBuilder builder;

	builder.write<ResetChip>(ResetChipOnDLS(), ResetChip(true));
	builder.write<Timer>(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10));
	builder.write<ResetChip>(ResetChipOnDLS(), ResetChip(false));
	builder.wait_until(TimerOnDLS(), Timer::Value(100));

	builder.write<JTAGClockScaler>(
	    JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write<ResetJTAGTap>(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// wait until Omnibus is up (22 us)
	builder.wait_until(TimerOnDLS(), Timer::Value(22 * fpga_clock_cycles_per_us));

	builder.write(address, config);

	return builder.done();
}

std::tuple<std::shared_ptr<PlaybackProgram>, PlaybackProgram::ContainerTicket<OmnibusChipOverJTAG>>
get_read_program(OmnibusChipOverJTAGAddress address)
{
	PlaybackProgramBuilder builder;

	auto ticket = builder.read(address);

	builder.write<Timer>(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10000));
	return std::make_tuple(builder.done(), ticket);
}

TEST(PlaybackProgram, Rerun)
{
	// Omnibus address of SystimeSyncBase
	auto address = OmnibusChipOverJTAGAddress(0x0);

	OmnibusChipOverJTAG config_1;
	config_1.set(OmnibusData(0x1));
	auto program_write_1 = get_write_program(address, config_1);

	OmnibusChipOverJTAG config_2;
	config_2.set(OmnibusData(0x2));
	auto program_write_2 = get_write_program(address, config_2);

	auto program_n_ticket = get_read_program(address);
	auto read_program = std::get<0>(program_n_ticket);
	auto read_ticket = std::get<1>(program_n_ticket);

	auto executor = generate_playback_program_test_executor();
	executor.run(program_write_1);
	executor.run(read_program);
	EXPECT_EQ(read_ticket.get(), config_1);

	executor.run(program_write_2);
	executor.run(read_program);
	EXPECT_EQ(read_ticket.get(), config_2);
}
