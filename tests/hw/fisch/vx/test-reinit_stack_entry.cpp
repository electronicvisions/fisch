#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reinit_stack_entry.h"
#include "fisch/vx/run.h"
#include "halco/hicann-dls/vx/barrier.h"
#include "halco/hicann-dls/vx/omnibus.h"
#include "hxcomm/vx/connection_from_env.h"

using namespace fisch::vx;
using namespace halco::hicann_dls::vx;

TEST(ReinitStackEntry, Register)
{
	auto empty_program = PlaybackProgramBuilder().done();
	auto connection = hxcomm::vx::get_connection_from_env();

	ReinitStackEntry reinit1{connection};
	run(connection, empty_program);

	reinit1.set(empty_program);

	{
		ReinitStackEntry reinit2{connection};
		reinit2.set(empty_program);

		{
			ReinitStackEntry reinit3{connection};

			reinit3.set(empty_program);

			run(connection, empty_program);
		}
		run(connection, empty_program);
	}
	run(connection, empty_program);
}

TEST(ReinitStackEntry, Snapshot)
{
	PlaybackProgramBuilder builder;

	// use first word in extmem for test
	builder.write(external_ppu_memory_base_address, Omnibus(Omnibus::Value(0x12345678)));
	auto program_write_1 = builder.done();

	builder.write(external_ppu_memory_base_address, Omnibus(Omnibus::Value(0x87654321)));
	auto program_write_2 = builder.done();

	auto ticket_read_1 = builder.read(external_ppu_memory_base_address);
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program_read_1 = builder.done();

	auto ticket_read_2 = builder.read(external_ppu_memory_base_address);
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program_read_2 = builder.done();

	auto ticket_read_2_after_write = builder.read(external_ppu_memory_base_address);
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program_read_2_after_write = builder.done();

	auto connection_1 = hxcomm::vx::get_connection_from_env();
	if (!std::holds_alternative<hxcomm::vx::QuiggeldyConnection>(connection_1)) {
		GTEST_SKIP() << "Snapshot test only works with quiggeldy.";
	}
	auto connection_2 = hxcomm::vx::get_connection_from_env();

	builder.read(external_ppu_memory_base_address);
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program_reinit_snapshot_1 = builder.done();
	auto program_reinit_request_1 = builder.done(); // empty

	ReinitStackEntry reinit_1{connection_1};
	reinit_1.set(program_reinit_request_1, program_reinit_snapshot_1);

	run(connection_1, program_write_1);            // write 0x12345678
	run(connection_2, program_write_2);            // snapshot 1, write 0x87654321
	run(connection_2, program_read_2_after_write); // read 0x87654321
	run(connection_1, program_read_1);             // apply snapshot 1, read 0x12345678
	run(connection_2, program_read_2);             // read 0x12345678, no snapshot 2

	auto read_2_after_write = ticket_read_2_after_write.get().at(0);
	auto read_1 = ticket_read_1.get().at(0);
	auto read_2 = ticket_read_1.get().at(0);

	EXPECT_EQ(read_2_after_write.get().word.value(), 0x87654321); // from program_write_2
	EXPECT_EQ(read_1.get().word.value(), 0x12345678);             // snapshot worked
	EXPECT_EQ(read_2.get().word.value(), 0x12345678); // no snapshot present, but program_write_2
	                                                  // was overwritten by snapshot of connection_1
}
