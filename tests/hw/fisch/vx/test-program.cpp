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

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

std::shared_ptr<PlaybackProgram> get_write_program(
    OmnibusChipOverJTAGAddress address, OmnibusChipOverJTAG config)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(true)));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(false)));
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100)));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// wait until Omnibus is up (22 us)
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(22 * fpga_clock_cycles_per_us)));

	builder.write(address, config);
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::jtag));

	return builder.done();
}

std::tuple<std::shared_ptr<PlaybackProgram>, ContainerTicket<OmnibusChipOverJTAG>> get_read_program(
    OmnibusChipOverJTAGAddress address)
{
	PlaybackProgramBuilder builder;

	auto ticket = builder.read(address);

	builder.write(TimerOnDLS(), Timer());
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::jtag));
	return std::make_tuple(builder.done(), ticket);
}

TEST(PlaybackProgram, Rerun)
{
	// Omnibus address of SystimeSyncBase
	auto address = OmnibusChipOverJTAGAddress(0x0);

	OmnibusChipOverJTAG config_1;
	config_1.set(OmnibusChipOverJTAG::Value(0x1));
	auto program_write_1 = get_write_program(address, config_1);

	OmnibusChipOverJTAG config_2;
	config_2.set(OmnibusChipOverJTAG::Value(0x2));
	auto program_write_2 = get_write_program(address, config_2);

	auto program_n_ticket = get_read_program(address);
	auto read_program = std::get<0>(program_n_ticket);
	auto read_ticket = std::get<1>(program_n_ticket);

	auto connection = hxcomm::vx::get_connection_from_env();
	run(connection, program_write_1);
	run(connection, read_program);
	EXPECT_EQ(read_ticket.get().at(0), config_1);

	run(connection, program_write_2);
	run(connection, read_program);
	EXPECT_EQ(read_ticket.get().at(0), config_2);
}

TEST(PlaybackProgramBuilder, MergeBack)
{
	// Omnibus address of SystimeSyncBase
	auto const address = OmnibusChipOverJTAGAddress(0x0);
	std::vector<OmnibusChipOverJTAGAddress> vector_address;
	vector_address.push_back(address);

	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(true)));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(false)));
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100)));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// wait until Omnibus is up (22 us)
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(22 * fpga_clock_cycles_per_us)));

	OmnibusChipOverJTAG config_1;
	config_1.set(OmnibusChipOverJTAG::Value(0x1));
	builder.write(address, config_1);

	auto ticket_1 = builder.read(address);
	auto vector_ticket_1 = builder.read(vector_address);

	OmnibusChipOverJTAG config_2;
	config_2.set(OmnibusChipOverJTAG::Value(0x2));
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
	config_3.set(OmnibusChipOverJTAG::Value(0x3));
	other.write(address, config_3);

	builder.merge_back(other);

	auto empty_program = PlaybackProgramBuilder().done();
	EXPECT_EQ(*(other.done()), *empty_program);

	auto ticket_3 = builder.read(address);
	auto vector_ticket_3 = builder.read(vector_address);

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::jtag));

	auto program = builder.done();

	auto connection = hxcomm::vx::get_connection_from_env();
	run(connection, program);

	EXPECT_TRUE(ticket_1.valid());
	EXPECT_TRUE(ticket_2.valid());
	EXPECT_TRUE(ticket_2_copy.valid());
	EXPECT_TRUE(ticket_3.valid());
	EXPECT_TRUE(vector_ticket_1.valid());
	EXPECT_TRUE(vector_ticket_2.valid());
	EXPECT_TRUE(vector_ticket_2_copy.valid());
	EXPECT_TRUE(vector_ticket_3.valid());

	EXPECT_EQ(ticket_1.get().at(0), config_1);
	EXPECT_EQ(ticket_2.get().at(0), config_2);
	EXPECT_EQ(ticket_2_copy.get().at(0), config_2);
	EXPECT_EQ(ticket_3.get().at(0), config_3);
	EXPECT_EQ(vector_ticket_1.get().at(0), config_1);
	EXPECT_EQ(vector_ticket_2.get().at(0), config_2);
	EXPECT_EQ(vector_ticket_2_copy.get().at(0), config_2);
	EXPECT_EQ(vector_ticket_3.get().at(0), config_3);
}
