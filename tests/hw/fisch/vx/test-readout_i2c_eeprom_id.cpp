#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/run.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/connection_from_env.h"

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

// Disabled due to missing hardware support (Issue #3645)
TEST(I2CIdRegister, DISABLED_Readout)
{
	PlaybackProgramBuilder builder;

	builder.write(i2c_prescaler_base_address, Omnibus(Omnibus::Value(313)));
	auto ticket = builder.read(I2CIdRegisterOnBoard());

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	auto connection = hxcomm::vx::get_connection_from_env();
	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	// FIXME: static value check might be nice but is bound to specific base-board.
	EXPECT_NE(ticket.get().at(0).get(), 0x0000'0000);
	EXPECT_NE(ticket.get().at(0).get(), 0xffff'ffff);
}
