#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/i2c.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/run.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hwdb4cpp/hwdb4cpp.h"
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

	auto const connection_unique_identifier =
	    std::visit([](auto const& conn) { return conn.get_unique_identifier(); }, connection);

	auto const [hxcube_id, fpga_id, _, __] =
	    hwdb4cpp::HXCubeSetupEntry::get_ids_from_unique_branch_identifier(
	        connection_unique_identifier);

	hwdb4cpp::database hwdb;
	hwdb.load(hwdb4cpp::database::get_default_path());
	auto& hxcube_setup_entry = hwdb.get_hxcube_setup_entry(hxcube_id);
	if (!hxcube_setup_entry.fpgas.contains(fpga_id)) {
		throw std::runtime_error("HXCubeSetupEntry doesn't feature FPGA ID from connection.");
	}
	if (!hxcube_setup_entry.fpgas[fpga_id].wing.has_value()) {
		throw std::runtime_error("HXCubeFPGAEntry doesn't feature wing.");
	}
	if (hxcube_setup_entry.fpgas[fpga_id].wing.value().handwritten_chip_serial < 75) {
		// old chip carrier version, no temperature sensor available
		return;
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
