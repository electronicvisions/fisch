#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/playback_executor.h"
#include "hxcomm/vx/simconnection.h"


extern std::string simulation_ip;
extern int simulation_port;

TEST(OmnibusOnChipOverJTAG, PPUWriteRead)
{
	fisch::vx::PlaybackProgramBuilder builder;

	builder.reset();
	builder.jtag_init();
	// wait until Omnibus is up (22 us)
	builder.wait_until(22 * fisch::vx::fpga_clock_cycles_per_us);

	fisch::vx::OmnibusOnChipOverJTAG config;
	// random data
	config.set(fisch::vx::OmnibusData(10));

	// Omnibus address of first PPU memory word of top PPU
	halco::hicann_dls::vx::OmnibusAddress addr(0x02000000 + (1 << 23));

	builder.write(addr, config);
	auto ticket = builder.read<fisch::vx::OmnibusOnChipOverJTAG>(addr);
	EXPECT_FALSE(ticket.valid());

	builder.wait_until(10000);
	builder.halt();
	auto program = builder.done();

	fisch::vx::PlaybackProgramExecutor<hxcomm::vx::SimConnection> executor(
	    simulation_ip, simulation_port);
	executor.transfer(program);
	executor.execute();
	executor.fetch(program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get(), config);
}
