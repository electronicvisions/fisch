#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/timer.h"

#include "executor.h"

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

std::shared_ptr<PlaybackProgram> get_write_program(
    OmnibusChipOverJTAGAddress address, OmnibusChipOverJTAG config)
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

	builder.write(address, config);

	return builder.done();
}

std::tuple<std::shared_ptr<PlaybackProgram>, PlaybackProgram::ContainerTicket<OmnibusChipOverJTAG>>
get_read_program(OmnibusChipOverJTAGAddress address)
{
	PlaybackProgramBuilder builder;

	auto ticket = builder.read(address);

	builder.write(TimerOnDLS(), Timer());
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

TEST(PlaybackProgramBuilder, MergeBack)
{
	// Omnibus address of SystimeSyncBase
	auto const address = OmnibusChipOverJTAGAddress(0x0);
	std::vector<OmnibusChipOverJTAGAddress> vector_address;
	vector_address.push_back(address);

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

	OmnibusChipOverJTAG config_1;
	config_1.set(OmnibusData(0x1));
	builder.write(address, config_1);

	auto ticket_1 = builder.read(address);
	auto vector_ticket_1 = builder.read(vector_address);

	OmnibusChipOverJTAG config_2;
	config_2.set(OmnibusData(0x2));
	builder.write(address, config_2);

	PlaybackProgramBuilder other;

	auto ticket_2 = other.read(address);
	auto vector_ticket_2 = other.read(vector_address);
	auto ticket_2_copy = ticket_2;
	auto vector_ticket_2_copy = vector_ticket_2;

	{
		auto destructed_ticket = other.read(address);
		auto destructed_vector_ticket = other.read(vector_address);
	}

	OmnibusChipOverJTAG config_3;
	config_3.set(OmnibusData(0x3));
	other.write(address, config_3);

	builder.merge_back(other);

	auto empty_program = PlaybackProgramBuilder().done();
	EXPECT_EQ(*(other.done()), *empty_program);

	auto ticket_3 = builder.read(address);
	auto vector_ticket_3 = builder.read(vector_address);

	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10000));

	auto program = builder.done();

	auto executor = generate_playback_program_test_executor();
	executor.run(program);

	EXPECT_TRUE(ticket_1.valid());
	EXPECT_TRUE(ticket_2.valid());
	EXPECT_TRUE(ticket_2_copy.valid());
	EXPECT_TRUE(ticket_3.valid());
	EXPECT_TRUE(vector_ticket_1.valid());
	EXPECT_TRUE(vector_ticket_2.valid());
	EXPECT_TRUE(vector_ticket_2_copy.valid());
	EXPECT_TRUE(vector_ticket_3.valid());

	EXPECT_EQ(ticket_1.get(), config_1);
	EXPECT_EQ(ticket_2.get(), config_2);
	EXPECT_EQ(ticket_2_copy.get(), config_2);
	EXPECT_EQ(ticket_3.get(), config_3);
	EXPECT_EQ(vector_ticket_1.get().at(0), config_1);
	EXPECT_EQ(vector_ticket_2.get().at(0), config_2);
	EXPECT_EQ(vector_ticket_2_copy.get().at(0), config_2);
	EXPECT_EQ(vector_ticket_3.get().at(0), config_3);
}
