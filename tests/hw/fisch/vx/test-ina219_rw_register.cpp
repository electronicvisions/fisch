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
#include "hxcomm/vx/simconnection.h"

#include "connection.h"

using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

TEST(I2CINA219RwRegister, Rw)
{
	PlaybackProgramBuilder builder;

	I2CINA219RwRegister config;
	config.set(I2CINA219RwRegister::Value(0x1234));
	I2CINA219RwRegisterOnBoard coord(I2CINA219RwRegisterOnINA219::calibration, INA219OnBoard());

	builder.write(i2c_prescaler_base_address, Omnibus(Omnibus::Value(313)));
	builder.write(coord, config);
	auto ticket = builder.read(coord);

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	auto connection = generate_test_connection();
	if constexpr (std::is_same_v<decltype(connection), hxcomm::vx::SimConnection>) {
		GTEST_SKIP() << "INA219 Register write read test only works in hardware.";
	}
	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	EXPECT_EQ(ticket.get().at(0), config);
}
