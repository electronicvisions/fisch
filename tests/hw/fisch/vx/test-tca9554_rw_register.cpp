#include <variant>
#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/run.h"
#include "halco/hicann-dls/vx/barrier.h"
#include "halco/hicann-dls/vx/i2c.h"
#include "hxcomm/vx/connection_from_env.h"
#include "hxcomm/vx/simconnection.h"

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

// We currently do not have the chip accessible in normal setups
TEST(I2CTCA9554RwRegister, DISABLED_Rw)
{
	PlaybackProgramBuilder builder;

	I2CTCA9554RwRegister config;
	config.set(I2CTCA9554RwRegister::Value(0xC0));
	I2CTCA9554RwRegisterOnBoard coord(I2CTCA9554RwRegisterOnTCA9554::config, TCA9554OnBoard());

	builder.write(i2c_prescaler_base_address, Omnibus(Omnibus::Value(313)));
	builder.write(coord, config);
	auto ticket = builder.read(coord);

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	auto connection = hxcomm::vx::get_connection_from_env();
	if (std::holds_alternative<hxcomm::vx::MultiSimConnection>(connection)) {
		GTEST_SKIP() << "TCA9554 Register write read test only works in hardware.";
	}
	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get().at(0), config);
}
