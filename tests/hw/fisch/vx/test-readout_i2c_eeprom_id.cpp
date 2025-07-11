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

TEST(I2CIdRegister, Readout)
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
	auto ticket = builder.read(I2CIdRegisterOnBoard());

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	run(connection, program);

	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());

	auto const connection_unique_identifier =
	    std::visit([](auto const& conn) { return conn.get_unique_identifier(); }, connection);

	auto const [hxcube_id, fpga_id, _, __] =
	    hwdb4cpp::HXCubeSetupEntry::get_ids_from_unique_branch_identifier(
	        connection_unique_identifier.at(0));

	hwdb4cpp::database hwdb;
	hwdb.load(hwdb4cpp::database::get_default_path());
	auto const& hxcube_setup_entry = hwdb.get_hxcube_setup_entry(hxcube_id);
	if (!hxcube_setup_entry.fpgas.contains(fpga_id)) {
		throw std::runtime_error("HXCubeSetupEntry doesn't feature FPGA ID from connection.");
	}
	if (!hxcube_setup_entry.fpgas.at(fpga_id).wing) {
		throw std::runtime_error("HXCubeSetupEntry doesn't feature chip.");
	}
	if (!hxcube_setup_entry.fpgas.at(fpga_id).wing->eeprom_chip_serial) {
		throw std::runtime_error("HXCubeSetupEntry doesn't feature chip eeprom serial.");
	}
	EXPECT_EQ(
	    ticket.get().at(0).get().value(),
	    *(hxcube_setup_entry.fpgas.at(fpga_id).wing->eeprom_chip_serial));
}
