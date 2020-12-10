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

TEST(I2CDAC6573RwRegister, DISABLED_Rw)
{
	PlaybackProgramBuilder builder;

	I2CDAC6573RwRegister config;
	config.set(I2CDAC6573RwRegister::Value(1023));
	I2CDAC6573RwRegisterOnBoard coord(
	    (I2CDAC6573RwRegisterOnBoard(DAC6573ChannelOnBoard::v_res_meas)));

	builder.write(i2c_prescaler_base_address, Omnibus(Omnibus::Value(313)));
	builder.write(coord, config);
	auto ticket = builder.read(coord);

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	auto connection = hxcomm::vx::get_connection_from_env();
	if (std::holds_alternative<hxcomm::vx::SimConnection>(connection)) {
		GTEST_SKIP() << "DAC6573 Register write read test only works in hardware.";
	}
	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get().at(0), config);
}
