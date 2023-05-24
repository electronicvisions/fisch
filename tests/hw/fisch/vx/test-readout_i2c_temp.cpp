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
#include "hxcomm/vx/target.h"

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

TEST(I2CTempRegister, Readout)
{
	auto connection = hxcomm::vx::get_connection_from_env();

	if (std::visit(
	        [](auto& conn) {
		        auto const supported_targets = std::set(conn.supported_targets);
		        return !supported_targets.contains(hxcomm::vx::Target::hardware);
	        },
	        connection)) {
		GTEST_SKIP() << "Connection does not support execution on hardware.";
	}

	PlaybackProgramBuilder builder;

	builder.write(i2c_prescaler_base_address, Omnibus(Omnibus::Value(313)));
	auto ticket = builder.read(I2CTempRegisterOnBoard());

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
}
